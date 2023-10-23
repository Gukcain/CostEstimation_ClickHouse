#pragma once

#include <Processors/IProcessor.h>
#include <base/unit.h>
#include <Processors/Chunk.h>
#include <Common/logger_useful.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal72{
    public:
        // T value;

        // const Block
        Block header;
        size_t num_ports;
        String order;
        size_t size_to_wait;

        // ParaVal72();
};
/*
 * Processor with N inputs and N outputs. Moves data from i-th input to i-th output as is.
 * It has a pair of auxiliary ports to notify another instance by sending empty chunk after some condition holds.
 * You should use this processor in pair of instances and connect auxiliary ports crosswise.
 *
 *     ╭─┴───┴───┴───┴───┴─╮       ╭─┴───┴───┴───┴───┴─╮
 *     │                   ├─ aux ⟶│                   │
 *     │ PingPongProcessor │       │ PingPongProcessor │
 *     │                   │⟵ aux ─┤                   │
 *     ╰─┬───┬───┬───┬───┬─╯       ╰─┬───┬───┬───┬───┬─╯
 *
 * One of the processors starts processing data, and another waits for notification.
 * When `consume` returns true, the first stops processing, sends a ping to another and waits for notification.
 * After that, the second one also processes data until `consume`, then send a notification back to the first one.
 * After this roundtrip, processors bypass data from regular inputs to outputs.
 */
class PingPongProcessor : public IProcessor
{
public:
    enum class Order : uint8_t
    {
        /// Processor that starts processing data.
        First,
        /// Processor that waits for notification.
        Second,
    };

    using enum Order;

    ParaVal72 pv72 = ParaVal72();
    std::vector<Param> getParaList() override{
        // ParaVal pv72 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv72.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv72.header.columns())));
        vec.push_back(Param("num_ports",std::to_string(pv72.num_ports)));
        vec.push_back(Param("order",pv72.order));
        return vec;
    }
    PingPongProcessor(const Block & header, size_t num_ports, Order order_);

    Status prepare() override;

    std::pair<InputPort *, OutputPort *> getAuxPorts();

    /// Returns `true` when enough data consumed
    virtual bool consume(const Chunk & chunk) = 0;

protected:
    struct PortsPair
    {
        InputPort * input_port = nullptr;
        OutputPort * output_port = nullptr;
        bool is_finished = false;
    };

    bool sendPing();
    bool recievePing();
    bool canSend() const;

    bool isPairsFinished() const;
    bool processPair(PortsPair & pair);
    void finishPair(PortsPair & pair);
    Status processRegularPorts();

    std::vector<PortsPair> port_pairs;
    size_t num_finished_pairs = 0;

    InputPort & aux_in_port;
    OutputPort & aux_out_port;

    bool is_send = false;
    bool is_received = false;

    bool ready_to_send = false;

    /// Used to set 'needed' flag once for auxiliary input at first `prepare` call.
    bool set_needed_once = false;

    Order order;
};

/// Reads first N rows from two streams evenly.
class ReadHeadBalancedProcessor : public PingPongProcessor
{
public:
    ParaVal72 pv722 = ParaVal72();
    std::vector<Param> getParaList() override{
        // ParaVal pv72 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv722.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv722.header.columns())));
        vec.push_back(Param("num_ports",std::to_string(pv722.num_ports)));
        vec.push_back(Param("size_to_wait",std::to_string(pv722.size_to_wait)));
        vec.push_back(Param("order",pv72.order));
        return vec;
    }
    ReadHeadBalancedProcessor(const Block & header, size_t num_ports, size_t size_to_wait_, Order order_)
        : PingPongProcessor(header, num_ports, order_) , data_consumed(0) , size_to_wait(size_to_wait_)
    {
        pv722.header = header;
        pv722.num_ports = num_ports;
        pv722.order = (order_==Order::First)?"First":"Second";
        pv722.size_to_wait = size_to_wait_;
    
    }

    String getName() const override { return "ReadHeadBalancedProcessor"; }

    bool consume(const Chunk & chunk) override
    {
        data_consumed += chunk.getNumRows();
        return data_consumed > size_to_wait;
    }

private:
    size_t data_consumed;
    size_t size_to_wait;
};

}
