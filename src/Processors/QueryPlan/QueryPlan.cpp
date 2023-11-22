#include <stack>

#include <Common/JSONBuilder.h>

#include <Interpreters/ActionsDAG.h>
#include <Interpreters/ArrayJoinAction.h>

#include <IO/Operators.h>
#include <IO/WriteBuffer.h>

#include <Processors/QueryPlan/BuildQueryPipelineSettings.h>
#include <Processors/QueryPlan/IQueryPlanStep.h>
#include <Processors/QueryPlan/Optimizations/Optimizations.h>
#include <Processors/QueryPlan/Optimizations/QueryPlanOptimizationSettings.h>
#include <Processors/QueryPlan/QueryPlan.h>
#include <Processors/QueryPlan/ReadFromMergeTree.h>

#include <QueryPipeline/QueryPipelineBuilder.h>

#include <fstream>
#include "Processors/json.hpp"
// using namespace std;
using json = nlohmann::json;


namespace ns{
    struct Frame
    {
        DB::QueryPlan::Node * node = {};
        size_t offset = 0;
        bool is_description_printed = false;
        size_t next_child = 0;
    };

    struct Processor{
        String name;
        std::vector<String> params;
    };
    struct NewNode{
        String address;
        std::vector<Processor> p = {};
        std::vector<String> children = {};
    };

    void to_json(nlohmann::json& j, const ns::Processor& val)
    {
        // j["id"] = val.id;
        // j["name"] = val.name;
        // j["maxPlayers"] = val.maxPlayers;
        // j["questionCount"] = val.questionCount;
        // j["timePerQuestion"] = val.timePerQuestion;
        // j["isActive"] = val.isActive;
        j["Name"]=val.name;
        j["Parameters"]=val.params;
    }

    void to_json(nlohmann::json& j, const ns::NewNode& val){
        j["Address"] = val.address;
        
    }
}

namespace DB
{

namespace ErrorCodes
{
    extern const int LOGICAL_ERROR;
}

QueryPlan::QueryPlan() = default;
QueryPlan::~QueryPlan() = default;
QueryPlan::QueryPlan(QueryPlan &&) noexcept = default;
QueryPlan & QueryPlan::operator=(QueryPlan &&) noexcept = default;

void QueryPlan::checkInitialized() const
{
    if (!isInitialized())
        throw Exception("QueryPlan was not initialized", ErrorCodes::LOGICAL_ERROR);
}

void QueryPlan::checkNotCompleted() const
{
    if (isCompleted())
        throw Exception("QueryPlan was already completed", ErrorCodes::LOGICAL_ERROR);
}

bool QueryPlan::isCompleted() const
{
    return isInitialized() && !root->step->hasOutputStream();
}

const DataStream & QueryPlan::getCurrentDataStream() const
{
    checkInitialized();
    checkNotCompleted();
    return root->step->getOutputStream();
}

void QueryPlan::unitePlans(QueryPlanStepPtr step, std::vector<std::unique_ptr<QueryPlan>> plans)
{
    if (isInitialized())
        throw Exception("Cannot unite plans because current QueryPlan is already initialized",
                        ErrorCodes::LOGICAL_ERROR);

    const auto & inputs = step->getInputStreams();
    size_t num_inputs = step->getInputStreams().size();
    if (num_inputs != plans.size())
        throw Exception(
            ErrorCodes::LOGICAL_ERROR,
            "Cannot unite QueryPlans using {} because step has different number of inputs. Has {} plans and {} inputs",
            step->getName(),
            plans.size(),
            num_inputs);

    for (size_t i = 0; i < num_inputs; ++i)
    {
        const auto & step_header = inputs[i].header;
        const auto & plan_header = plans[i]->getCurrentDataStream().header;
        if (!blocksHaveEqualStructure(step_header, plan_header))
            throw Exception(
                ErrorCodes::LOGICAL_ERROR,
                "Cannot unite QueryPlans using {} because it has incompatible header with plan {} plan header: {} step header: {}",
                step->getName(),
                root->step->getName(),
                plan_header.dumpStructure(),
                step_header.dumpStructure());
    }

    for (auto & plan : plans)
        nodes.splice(nodes.end(), std::move(plan->nodes));

    nodes.emplace_back(Node{.step = std::move(step)});
    root = &nodes.back();

    for (auto & plan : plans)
        root->children.emplace_back(plan->root);

    for (auto & plan : plans)
    {
        max_threads = std::max(max_threads, plan->max_threads);
        resources = std::move(plan->resources);
    }
}

void QueryPlan::addStep(QueryPlanStepPtr step)
{
    checkNotCompleted();

    size_t num_input_streams = step->getInputStreams().size();

    if (num_input_streams == 0)
    {
        if (isInitialized())
            throw Exception(
                ErrorCodes::LOGICAL_ERROR,
                "Cannot add step {} to QueryPlan because step has no inputs, but QueryPlan is already initialized",
                step->getName());

        nodes.emplace_back(Node{.step = std::move(step)});
        root = &nodes.back();
        return;
    }

    if (num_input_streams == 1)
    {
        if (!isInitialized())
            throw Exception(
                ErrorCodes::LOGICAL_ERROR,
                "Cannot add step {} to QueryPlan because step has input, but QueryPlan is not initialized",
                step->getName());

        const auto & root_header = root->step->getOutputStream().header;
        const auto & step_header = step->getInputStreams().front().header;
        if (!blocksHaveEqualStructure(root_header, step_header))
            throw Exception(
                ErrorCodes::LOGICAL_ERROR,
                "Cannot add step {} to QueryPlan because it has incompatible header with root step {} root header: {} step header: {}",
                step->getName(),
                root->step->getName(),
                root_header.dumpStructure(),
                step_header.dumpStructure());

        nodes.emplace_back(Node{.step = std::move(step), .children = {root}});
        root = &nodes.back();
        return;
    }

    throw Exception(
        ErrorCodes::LOGICAL_ERROR,
        "Cannot add step {} to QueryPlan because it has {} inputs but {} input expected",
        step->getName(),
        num_input_streams,
        isInitialized() ? 1 : 0);
}

QueryPipelineBuilderPtr QueryPlan::buildQueryPipeline(
    const QueryPlanOptimizationSettings & optimization_settings,
    const BuildQueryPipelineSettings & build_pipeline_settings)
{
    // 改 05-08
    // ofstream location_out;
    // location_out.open("RandomTest.txt", std::ios::out | std::ios::app); 
    // location_out << "进入了QueryPlan::buildQueryPipeline" << std::endl;
    // location_out.close();

    checkInitialized();
    optimize(optimization_settings);

    struct Frame
    {
        Node * node = {};
        QueryPipelineBuilders pipelines = {};
    };

    QueryPipelineBuilderPtr last_pipeline;

    std::stack<Frame> stack;
    stack.push(Frame{.node = root});

    while (!stack.empty())
    {
        auto & frame = stack.top();

        if (last_pipeline)
        {
            frame.pipelines.emplace_back(std::move(last_pipeline));
            last_pipeline = nullptr; //-V1048
        }

        size_t next_child = frame.pipelines.size();
        // 当一个Node中的children都组装成了Pipeline时，那么就可以当前Node与children组装好的
        // pipeline 构造新的Pipeline.
        if (next_child == frame.node->children.size())
        {
            bool limit_max_threads = frame.pipelines.empty();
            // 改 05-07
            // 回 2023-11-14
            // limit_max_threads = true;
            last_pipeline = frame.node->step->updatePipeline(std::move(frame.pipelines), build_pipeline_settings);

            if (limit_max_threads && max_threads)
                last_pipeline->limitMaxThreads(max_threads);

            stack.pop();
        }
        else
            stack.push(Frame{.node = frame.node->children[next_child]});
    }

    last_pipeline->setProgressCallback(build_pipeline_settings.progress_callback);
    last_pipeline->setProcessListElement(build_pipeline_settings.process_list_element);
    last_pipeline->addResources(std::move(resources));

    return last_pipeline;
}

static void explainStep(const IQueryPlanStep & step, JSONBuilder::JSONMap & map, const QueryPlan::ExplainPlanOptions & options)
{
    map.add("Node Type", step.getName());

    if (options.description)
    {
        const auto & description = step.getStepDescription();
        if (!description.empty())
            map.add("Description", description);
    }

    if (options.header && step.hasOutputStream())
    {
        auto header_array = std::make_unique<JSONBuilder::JSONArray>();

        for (const auto & output_column : step.getOutputStream().header)
        {
            auto column_map = std::make_unique<JSONBuilder::JSONMap>();
            column_map->add("Name", output_column.name);
            if (output_column.type)
                column_map->add("Type", output_column.type->getName());

            header_array->add(std::move(column_map));
        }

        map.add("Header", std::move(header_array));
    }

    if (options.actions)
        step.describeActions(map);

    if (options.indexes)
        step.describeIndexes(map);
}

JSONBuilder::ItemPtr QueryPlan::explainPlan(const ExplainPlanOptions & options)
{
    checkInitialized();

    struct Frame
    {
        Node * node = {};
        size_t next_child = 0;
        std::unique_ptr<JSONBuilder::JSONMap> node_map = {};
        std::unique_ptr<JSONBuilder::JSONArray> children_array = {};
    };

    std::stack<Frame> stack;
    stack.push(Frame{.node = root});

    std::unique_ptr<JSONBuilder::JSONMap> tree;

    while (!stack.empty())
    {
        auto & frame = stack.top();

        if (frame.next_child == 0)
        {
            if (!frame.node->children.empty())
                frame.children_array = std::make_unique<JSONBuilder::JSONArray>();

            frame.node_map = std::make_unique<JSONBuilder::JSONMap>();
            explainStep(*frame.node->step, *frame.node_map, options);
        }

        if (frame.next_child < frame.node->children.size())
        {
            stack.push(Frame{frame.node->children[frame.next_child]});
            ++frame.next_child;
        }
        else
        {
            if (frame.children_array)
                frame.node_map->add("Plans", std::move(frame.children_array));

            tree.swap(frame.node_map);
            stack.pop();

            if (!stack.empty())
                stack.top().children_array->add(std::move(tree));
        }
    }

    return tree;
}

static void explainStep(
    const IQueryPlanStep & step,
    IQueryPlanStep::FormatSettings & settings,
    const QueryPlan::ExplainPlanOptions & options)
{
    std::string prefix(settings.offset, ' ');
    settings.out << prefix;
    settings.out << step.getName();

    const auto & description = step.getStepDescription();
    if (options.description && !description.empty())
        settings.out <<" (" << description << ')';

    settings.out.write('\n');

    if (options.header)
    {
        settings.out << prefix;

        if (!step.hasOutputStream())
            settings.out << "No header";
        else if (!step.getOutputStream().header)
            settings.out << "Empty header";
        else
        {
            settings.out << "Header: ";
            bool first = true;

            for (const auto & elem : step.getOutputStream().header)
            {
                if (!first)
                    settings.out << "\n" << prefix << "        ";

                first = false;
                elem.dumpNameAndType(settings.out);
            }
        }
        settings.out.write('\n');

    }

    if (options.sorting)
    {
        if (step.hasOutputStream())
        {
            settings.out << prefix << "Sorting (" << step.getOutputStream().sort_scope << ")";
            if (step.getOutputStream().sort_scope != DataStream::SortScope::None)
            {
                settings.out << ": ";
                dumpSortDescription(step.getOutputStream().sort_description, settings.out);
            }
            settings.out.write('\n');
        }
    }

    if (options.actions)
        step.describeActions(settings);

    if (options.indexes)
        step.describeIndexes(settings);
}

std::string debugExplainStep(const IQueryPlanStep & step)
{
    WriteBufferFromOwnString out;
    IQueryPlanStep::FormatSettings settings{.out = out};
    QueryPlan::ExplainPlanOptions options{.actions = true};
    explainStep(step, settings, options);
    return out.str();
}

void QueryPlan::explainPlan(WriteBuffer & buffer, const ExplainPlanOptions & options)
{
    checkInitialized();

    IQueryPlanStep::FormatSettings settings{.out = buffer, .write_header = options.header};

    struct Frame
    {
        Node * node = {};
        bool is_description_printed = false;
        size_t next_child = 0;
    };

    std::stack<Frame> stack;
    stack.push(Frame{.node = root});

    while (!stack.empty())
    {
        auto & frame = stack.top();

        if (!frame.is_description_printed)
        {
            settings.offset = (stack.size() - 1) * settings.indent;
            explainStep(*frame.node->step, settings, options);
            frame.is_description_printed = true;
        }

        if (frame.next_child < frame.node->children.size())
        {
            stack.push(Frame{frame.node->children[frame.next_child]});
            ++frame.next_child;
        }
        else
            stack.pop();
    }
}

static void explainPipelineStep(IQueryPlanStep & step, IQueryPlanStep::FormatSettings & settings)
{
    settings.out << String(settings.offset, settings.indent_char) << "(" << step.getName() << ")\n";

    size_t current_offset = settings.offset;
    step.describePipeline(settings);
    if (current_offset == settings.offset)
        settings.offset += settings.indent;
}

void QueryPlan::explainPipeline(WriteBuffer & buffer, const ExplainPipelineOptions & options)
{
    checkInitialized();

    IQueryPlanStep::FormatSettings settings{.out = buffer, .write_header = options.header};

    struct Frame
    {
        Node * node = {};
        size_t offset = 0;
        bool is_description_printed = false;
        size_t next_child = 0;
    };

    std::stack<Frame> stack;
    stack.push(Frame{.node = root});

    while (!stack.empty())
    {
        auto & frame = stack.top();

        if (!frame.is_description_printed)
        {
            settings.offset = frame.offset;
            explainPipelineStep(*frame.node->step, settings);
            frame.offset = settings.offset;
            frame.is_description_printed = true;
        }

        if (frame.next_child < frame.node->children.size())
        {
            stack.push(Frame{frame.node->children[frame.next_child], frame.offset});
            ++frame.next_child;
        }
        else
            stack.pop();
    }
}

void QueryPlan::outputPipeline()
{
    checkInitialized();
    WriteBufferFromOwnString buffer;
    IQueryPlanStep::FormatSettings settings{.out = buffer, .write_header = false};

    json result;
    json all_nodes;

    std::stack<ns::Frame> stack;
    stack.push(ns::Frame{.node = root});
    std::vector<ns::NewNode> nodeslist;

    while (!stack.empty())
    {
        auto & frame = stack.top();

        if (!frame.is_description_printed)
        {
            settings.offset = frame.offset;
            // explainPipelineStep(*frame.node->step, settings);
        
            json node_j;
            json node_children_list;
            json node_processors_list;
            ns::NewNode node_n;
            IProcessor * prev = nullptr;
            
            // String addr;
            // char ptr_str[20];
            // std::sprintf(ptr_str, "0x%p", static_cast<void*>(frame.node)); 
            node_n.address = "0x" + std::to_string(*(static_cast<unsigned long long*>(static_cast<void*>(frame.node)))); //地址
            node_j["Address"] = node_n.address;

            for(Node* nd:frame.node->children){         //孩子
                String addr = "0x" + std::to_string(*(static_cast<unsigned long long*>(static_cast<void*>(nd))));
                node_n.children.push_back(addr);
                json json_child_addr;
                json_child_addr["Address"] = addr;
                node_children_list.push_back(json_child_addr);
            }
            // node_j["Children"] = node_children_list;

            // for(auto it = frame.node->children.begin();it != frame.node->children.end();++it){
            //     node_n.children.push_back("0x" + std::to_string((unsigned long long)static_cast<void*>(it->get())););
            // }

            //算子
            processorList.clear();
            (*frame.node->step).describePipeline(settings);
            for (auto it = processorList.rbegin(); it != processorList.rend(); ++it)
            {
                if (prev && prev->getName() != (*it)->getName())
                {

                    // doDescribeProcessor(*prev, count, settings);
                    // std::vector<String> s;
                    // for(Param p:prev->getParaList()){
                    //     s.push_back(p.val);
                    // }

                    std::vector<String> s;
                    s = prev->fillParamVec(prev->getParaList());
                    node_n.p.push_back(ns::Processor{.name = prev->getName(),.params = s});

                    json a_processor;
                    a_processor["Name"] = prev->getName();
                    // 改 11-12
                    a_processor["Address"] = "0x" + std::to_string(*(static_cast<unsigned long long*>(static_cast<void*>(prev))));
                    a_processor["Parameters"] = s;

                    node_processors_list.push_back(a_processor);

                }

                prev = it->get();
            }

            if (prev){
                // std::vector<String> s;
                // for(Param p:prev->getParaList()){
                //     s.push_back(p.val);
                // }
                std::vector<String> s;
                s = prev->fillParamVec(prev->getParaList());

                node_n.p.push_back(ns::Processor{.name = prev->getName(),.params = s});

                json a_processor;
                a_processor["Name"] = prev->getName();
                // 改 11-12
                a_processor["Address"] = "0x" + std::to_string(*(static_cast<unsigned long long*>(static_cast<void*>(prev))));
                a_processor["Parameters"] = s;

                node_processors_list.push_back(a_processor);
            }
                // doDescribeProcessor(*prev, count, settings);

            frame.offset = settings.offset;
            frame.is_description_printed = true;
            nodeslist.push_back(node_n);

            node_j["Processors"] = node_processors_list;
            node_j["Children"] = node_children_list;

            all_nodes.push_back(node_j);
        }

        if (frame.next_child < frame.node->children.size())
        {
            stack.push(ns::Frame{frame.node->children[frame.next_child], frame.offset});
            ++frame.next_child;
        }
        else
            stack.pop();
    }

    result["Query"] = Query_String;
    result["Nodes"] = all_nodes;
    // 改 11-12
    result["QueryID"] = query_seq; 
    
    std::ofstream o;
    o.open("PipelineTree.json",std::ios::out|std::ios::app);
    o << std::setw(4) << result << std::endl;

    // std::ofstream os;
    // os.open("ForTest.txt",std::ios::out|std::ios::app);
    // os<<5<<std::endl;
    // os.close();

}

void QueryPlan::optimize(const QueryPlanOptimizationSettings & optimization_settings)
{
    QueryPlanOptimizations::optimizeTreeFirstPass(optimization_settings, *root, nodes);
    QueryPlanOptimizations::optimizeTreeSecondPass(optimization_settings, *root, nodes);
}

void QueryPlan::explainEstimate(MutableColumns & columns)
{
    checkInitialized();

    struct EstimateCounters
    {
        std::string database_name;
        std::string table_name;
        UInt64 parts = 0;
        UInt64 rows = 0;
        UInt64 marks = 0;

        EstimateCounters(const std::string & database, const std::string & table) : database_name(database), table_name(table)
        {
        }
    };

    using CountersPtr = std::shared_ptr<EstimateCounters>;
    std::unordered_map<std::string, CountersPtr> counters;
    using processNodeFuncType = std::function<void(const Node * node)>;
    processNodeFuncType process_node = [&counters, &process_node] (const Node * node)
    {
        if (!node)
            return;
        if (const auto * step = dynamic_cast<ReadFromMergeTree*>(node->step.get()))
        {
            const auto & id = step->getStorageID();
            auto key = id.database_name + "." + id.table_name;
            auto it = counters.find(key);
            if (it == counters.end())
            {
                it = counters.insert({key, std::make_shared<EstimateCounters>(id.database_name, id.table_name)}).first;
            }
            it->second->parts += step->getSelectedParts();
            it->second->rows += step->getSelectedRows();
            it->second->marks += step->getSelectedMarks();
        }
        for (const auto * child : node->children)
            process_node(child);
    };
    process_node(root);

    for (const auto & counter : counters)
    {
        size_t index = 0;
        const auto & database_name = counter.second->database_name;
        const auto & table_name = counter.second->table_name;
        columns[index++]->insertData(database_name.c_str(), database_name.size());
        columns[index++]->insertData(table_name.c_str(), table_name.size());
        columns[index++]->insert(counter.second->parts);
        columns[index++]->insert(counter.second->rows);
        columns[index++]->insert(counter.second->marks);
    }
}

}
