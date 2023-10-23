#pragma once

#include <Core/Names.h>
#include <Interpreters/Context_fwd.h>
#include <Columns/IColumn.h>
#include <QueryPipeline/QueryPlanResourceHolder.h>

#include <list>
#include <memory>
#include <set>
#include <vector>

#include <stack>
#include <Common/JSONBuilder.h>
#include "Processors/QueryPlan/IQueryPlanStep.h"

#include "Processors/Param.h"
#include "Processors/json.hpp"

namespace DB
{

class DataStream;

class IQueryPlanStep;
using QueryPlanStepPtr = std::unique_ptr<IQueryPlanStep>;

class QueryPipelineBuilder;
using QueryPipelineBuilderPtr = std::unique_ptr<QueryPipelineBuilder>;

class WriteBuffer;

class QueryPlan;
using QueryPlanPtr = std::unique_ptr<QueryPlan>;

class Pipe;

struct QueryPlanOptimizationSettings;
struct BuildQueryPipelineSettings;

namespace JSONBuilder
{
    class IItem;
    using ItemPtr = std::unique_ptr<IItem>;
}

/// A tree of query steps.
/// The goal of QueryPlan is to build QueryPipeline.
/// QueryPlan let delay pipeline creation which is helpful for pipeline-level optimizations.
class QueryPlan
{
public:
    QueryPlan();
    ~QueryPlan();
    QueryPlan(QueryPlan &&) noexcept;
    QueryPlan & operator=(QueryPlan &&) noexcept;

    void unitePlans(QueryPlanStepPtr step, std::vector<QueryPlanPtr> plans);
    void addStep(QueryPlanStepPtr step);

    bool isInitialized() const { return root != nullptr; } /// Tree is not empty
    bool isCompleted() const; /// Tree is not empty and root hasOutputStream()
    const DataStream & getCurrentDataStream() const; /// Checks that (isInitialized() && !isCompleted())

    void optimize(const QueryPlanOptimizationSettings & optimization_settings);

    QueryPipelineBuilderPtr buildQueryPipeline(
        const QueryPlanOptimizationSettings & optimization_settings,
        const BuildQueryPipelineSettings & build_pipeline_settings);

    struct ExplainPlanOptions
    {
        /// Add output header to step.
        bool header = false;
        /// Add description of step.
        bool description = true;
        /// Add detailed information about step actions.
        bool actions = false;
        /// Add information about indexes actions.
        bool indexes = false;
        /// Add information about sorting
        bool sorting = false;
    };

    struct ExplainPipelineOptions
    {
        /// Show header of output ports.
        bool header = false;
    };

    // 改 08-09
    // void jsonOutputPlan(){
    //     checkInitialized();

    //     // struct ExplainPlanOptions
    //     // {
    //     //     /// Add output header to step.
    //     //     bool header = false;
    //     //     /// Add description of step.
    //     //     bool description = true;
    //     //     /// Add detailed information about step actions.
    //     //     bool actions = false;
    //     //     /// Add information about indexes actions.
    //     //     bool indexes = false;
    //     //     /// Add information about sorting
    //     //     bool sorting = false;
    //     // };
    //     ExplainPlanOptions options{.header=true,.actions=true};

    //     struct Frame
    //     {
    //         Node * node = {};
    //         size_t next_child = 0;
    //         std::unique_ptr<JSONBuilder::JSONMap> node_map = {};
    //         std::unique_ptr<JSONBuilder::JSONArray> children_array = {};
    //     };

    //     std::stack<Frame> stack;
    //     stack.push(Frame{.node = root});

    //     std::unique_ptr<JSONBuilder::JSONMap> tree;

    //     while (!stack.empty())
    //     {
    //         auto & frame = stack.top();

    //         if (frame.next_child == 0)
    //         {
    //             if (!frame.node->children.empty())
    //                 frame.children_array = std::make_unique<JSONBuilder::JSONArray>();

    //             frame.node_map = std::make_unique<JSONBuilder::JSONMap>();
    //             explainStep(*frame.node->step, *frame.node_map, options);
    //         }

    //         if (frame.next_child < frame.node->children.size())
    //         {
    //             stack.push(Frame{frame.node->children[frame.next_child]});
    //             ++frame.next_child;
    //         }
    //         else
    //         {
    //             if (frame.children_array)
    //                 frame.node_map->add("Plans", std::move(frame.children_array));

    //             tree.swap(frame.node_map);
    //             stack.pop();

    //             if (!stack.empty())
    //                 stack.top().children_array->add(std::move(tree));
    //         }
    //     }

    //     // return tree;
    // }

    // static void explainStep(const IQueryPlanStep & step, JSONBuilder::JSONMap & map, const QueryPlan::ExplainPlanOptions & options)
    // {
    //     map.add("Node Type", step.getName());

    //     if (options.description)
    //     {
    //         const auto & description = step.getStepDescription();
    //         if (!description.empty())
    //             map.add("Description", description);
    //     }

    //     if (options.header && step.hasOutputStream())
    //     {
    //         auto header_array = std::make_unique<JSONBuilder::JSONArray>();

    //         for (const auto & output_column : step.getOutputStream().header)
    //         {
    //             auto column_map = std::make_unique<JSONBuilder::JSONMap>();
    //             column_map->add("Name", output_column.name);
    //             if (output_column.type)
    //                 column_map->add("Type", output_column.type->getName());

    //             header_array->add(std::move(column_map));
    //         }

    //         map.add("Header", std::move(header_array));
    //     }
        
    //     map.add("Parameters", );

    //     if (options.actions)
    //         step.describeActions(map);

    //     if (options.indexes)
    //         step.describeIndexes(map);
    // }

    // static void explainStep(const IQueryPlanStep & step, JSONBuilder::JSONMap & map, const QueryPlan::ExplainPlanOptions & options);



    JSONBuilder::ItemPtr explainPlan(const ExplainPlanOptions & options);
    void explainPlan(WriteBuffer & buffer, const ExplainPlanOptions & options);
    void explainPipeline(WriteBuffer & buffer, const ExplainPipelineOptions & options);
    void outputPipeline();
    void explainEstimate(MutableColumns & columns);

    /// Do not allow to change the table while the pipeline alive.
    void addTableLock(TableLockHolder lock) { resources.table_locks.emplace_back(std::move(lock)); }
    void addInterpreterContext(std::shared_ptr<const Context> context) { resources.interpreter_context.emplace_back(std::move(context)); }
    void addStorageHolder(StoragePtr storage) { resources.storage_holders.emplace_back(std::move(storage)); }

    void addResources(QueryPlanResourceHolder resources_) { resources = std::move(resources_); }

    /// Set upper limit for the recommend number of threads. Will be applied to the newly-created pipelines.
    /// TODO: make it in a better way.
    void setMaxThreads(size_t max_threads_) { max_threads = max_threads_; }
    size_t getMaxThreads() const { return max_threads; }

    /// Tree node. Step and it's children.
    struct Node
    {
        QueryPlanStepPtr step;
        std::vector<Node *> children = {};
    };

    // struct Frame
    // {
    //     Node * node = {};
    //     size_t offset = 0;
    //     bool is_description_printed = false;
    //     size_t next_child = 0;
    // };

    // struct NewNode{
    //     String name;
    //     std::vector<Param> params;
    //     std::vector<NewNode *> children = {};
    // };

    const Node * getRootNode() const { return root; }

    using Nodes = std::list<Node>;

private:
    QueryPlanResourceHolder resources;
    Nodes nodes;
    Node * root = nullptr;

    void checkInitialized() const;
    void checkNotCompleted() const;

    /// Those fields are passed to QueryPipeline.
    size_t max_threads = 1;
};

std::string debugExplainStep(const IQueryPlanStep & step);

}
