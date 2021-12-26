/******************************************************************************
 * BIKE -- Bit Flipping Key Encapsulation
 *
 * Copyright (c) 2021 Nir Drucker, Shay Gueron, Rafael Misoczki, Tobias Oder,
 * Tim Gueneysu, Jan Richter-Brockmann.
 * Contact: drucker.nir@gmail.com, shay.gueron@gmail.com,
 * rafaelmisoczki@google.com, tobias.oder@rub.de, tim.gueneysu@rub.de,
 * jan.richter-brockmann@rub.de.
 *
 * Permission to use this code for BIKE is granted.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * * The names of the contributors may not be used to endorse or promote
 *   products derived from this software without specific prior written
 *   permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ""AS IS"" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS CORPORATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/


#ifndef MEASURE_H
#define MEASURE_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>

#ifndef REPEAT
#define REPEAT 10
#endif

#ifndef OUTER_REPEAT
#define OUTER_REPEAT 1
#endif

#ifndef RDTSC
//Less accurate measurement than with RDTSC
#include <time.h>
#include <chrono>


std::chrono::steady_clock::time_point start;
std::chrono::steady_clock::time_point end;


#define MEASURE(msg, x) start = std::chrono::steady_clock::now();\
		for (int i = 0; i < REPEAT; i++)   \
        {                                                                                             \
                {x};                                                                                       \
        }\
        end = std::chrono::steady_clock::now(); \
        printf(msg); \
        printf("\ttook %lfs (%d repetitions)\n", ((double) std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()/CLOCKS_PER_SEC), REPEAT);
#endif


/* This part defines the functions and MACROS needed to measure using RDTSC */
#ifdef RDTSC

#ifndef WARMUP
#define WARMUP REPEAT/4
#endif

unsigned long long RDTSC_start_clk, RDTSC_end_clk;
double RDTSC_total_clk;
double RDTSC_TEMP_CLK;
int RDTSC_MEASURE_ITERATOR;
int RDTSC_OUTER_ITERATOR;


inline static uint32_t get_Clks(void)
{
    uint32_t pmccntr;
 	
	asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(pmccntr));
	return (uint32_t)(pmccntr);
}

/*
   This MACRO measures the number of cycles "x" runs. This is the flow:
      1) it sets the priority to FIFO, to avoid time slicing if possible.
      2) it repeats "x" WARMUP times, in order to warm the cache.
      3) it reads the Time Stamp Counter at the beginning of the test.
      4) it repeats "x" REPEAT number of times.
      5) it reads the Time Stamp Counter again at the end of the test
      6) it calculates the average number of cycles per one iteration of "x", by calculating the total number of cycles, and dividing it by REPEAT
 */
#define RDTSC_MEASURE(msg, x)                                                                    \
        for(RDTSC_MEASURE_ITERATOR=0; RDTSC_MEASURE_ITERATOR< WARMUP; RDTSC_MEASURE_ITERATOR++)          \
        {                                                                                             \
            {x};                                                                                       \
        }                                                                                                \
        RDTSC_total_clk = 1.7976931348623157e+308;                                                      \
        for(RDTSC_OUTER_ITERATOR=0;RDTSC_OUTER_ITERATOR<OUTER_REPEAT; RDTSC_OUTER_ITERATOR++){          \
            RDTSC_start_clk = get_Clks();                                                                 \
            for (RDTSC_MEASURE_ITERATOR = 0; RDTSC_MEASURE_ITERATOR < REPEAT; RDTSC_MEASURE_ITERATOR++)   \
            {                                                                                             \
                {x};                                                                                       \
            }                                                                                             \
            RDTSC_end_clk = get_Clks();                                                                   \
            RDTSC_TEMP_CLK = (double)(RDTSC_end_clk-RDTSC_start_clk)/REPEAT;                              \
            if(RDTSC_total_clk>RDTSC_TEMP_CLK) RDTSC_total_clk = RDTSC_TEMP_CLK;                        \
        } printf(msg); \
        printf(" took %012.2f cycles in average (%d repetitions)\n", RDTSC_total_clk, REPEAT );


#ifndef COHO
#define MEASURE(msg, x) RDTSC_MEASURE(msg, x)
#endif

#endif

#endif
