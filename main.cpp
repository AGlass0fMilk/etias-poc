/**
 * etias test
 *
 * Date: 3/12/2020
 * Author: gdbeckstein
 *
 */

#include "OutputSignalNode.hpp"
#include "HysteresisBlock.hpp"

#include "platform/mbed_wait_api.h"

template<typename T, int SampleRate = SAMPLE_RATE_UNDEFINED>
class TriangleWaveSimulator {

public:

    TriangleWaveSimulator(T step, T min, T max) : step(step), min(min), max(max),
    current(min), going_up(true) {}

    void poll(void) {

        output_node.notify(current);

        if(going_up) {
            current += step;
        } else {
            current -= step;
        }

        if(current > max) {
            current = max-step;
            going_up = false;
        } else if (current < min) {
            current = min+step;
            going_up = true;
        }
    }

    etias::OutputSignalNode<T, SampleRate>& output(void) {
        return output_node;
    }

protected:

    T step;
    T min;
    T max;
    T current;

    bool going_up;

    etias::OutputSignalNode<T, SampleRate> output_node;
};

TriangleWaveSimulator<float> twave_sim(1.0f, 0.0f, 125.0f);
etias::HysteresisBlock<float> temp_hysteresis(35.0f, 45.0f);
etias::InputSignalNode<float> upper_threshold_input([](float a) {
    printf("crossed upper threshold! %.2f\r\n", a);
});
etias::InputSignalNode<float> lower_threshold_input([](float a) {
   printf("crossed lower threshold! %.2f\r\n", a);
});
etias::InputSignalNode<float> dump_input([](float a) {
    printf("updated value: %.2f\r\n", a);
});



int main(void) {

    twave_sim.output().connect(temp_hysteresis.input());
    twave_sim.output().connect(dump_input);
    temp_hysteresis.cross_lower().connect(lower_threshold_input);
    temp_hysteresis.cross_upper().connect(upper_threshold_input);

    while(true) {

        twave_sim.poll();

        wait_ms(10);

    }

}

