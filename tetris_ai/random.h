#pragma once
//#include <time.h>
#include <utility>
//************************************************************************
//  This is a slightly modified version of Equamen mersenne twister.
//
//  Copyright (C) 2009 Chipset
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License
//  along with this program. If not, see <http://www.gnu.org/licenses/>.
//************************************************************************

// Original Coyright (c) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura
//
// Functions for MT19937, with initialization improved 2002/2/10.
// Coded by Takuji Nishimura and Makoto Matsumoto.
// This is a faster version by taking Shawn Cokus's optimization,
// Matthe Bellew's simplification, Isaku Wada's real version.
// C++ version by Lyell Haynes (Equamen)

namespace AI {

const int N = 624;
const int M = 397;

typedef unsigned __int32 uint32;

class mtrandom {
public:
	mtrandom() : left(1) { init(); }
    mtrandom(const mtrandom& m) {
        for ( int i = 0; i < N; ++i) {
            state[i] = m.state[i];
        }
        left = m.left;
        next = state + (m.next - m.state);
    }

	explicit mtrandom(uint32 seed) : left(1) {    init(seed);    }

	mtrandom& operator=(const mtrandom& m) {
		for (int i = 0; i < N; ++i) {
			state[i] = m.state[i];
		}
		left = m.left;
		next = state + (m.next - m.state);
		return *this;
	}

	mtrandom(uint32* init_key, int key_length) : left(1) {
		int i = 1, j = 0;
		int k = N > key_length ? N : key_length;
		init();
		for(; k; --k) {
			state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1664525UL)) + init_key[j] + j; // non linear
			state[i] &= 4294967295UL; // for WORDSIZE > 32 machines
			++i;
			++j;
			if(i >= N) {
				state[0] = state[N - 1];
				i = 1;
			}
			if(j >= key_length)
				j = 0;
		}

		for(k = N - 1; k; --k) {
			state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1566083941UL)) - i; // non linear
			state[i] &= 4294967295UL; // for WORDSIZE > 32 machines
			++i;
			if(i >= N) {
				state[0] = state[N - 1];
				i = 1;
			}
		}

		state[0]  =  2147483648UL; // MSB is 1; assuring non-zero initial array
	}

	void reset(uint32 rs) {
		init(rs);
		next_state();
	}

	uint32 rand() {
		uint32 y;
		if(0 == --left)
			next_state();
		y  = *next++;
		// Tempering
		y ^= (y >> 11);
		y ^= (y << 7) & 0x9d2c5680UL;
		y ^= (y << 15) & 0xefc60000UL;
		y ^= (y >> 18);
		return y;
	}

	double real()    {    return (double)rand() / ((double)(unsigned long)(-1L) + 1);    }

	// generates a random number on [0,1) with 53-bit resolution
	double res53() {
		uint32 a = rand() >> 5, b = rand() >> 6;
		return (a * 67108864.0 + b) / 9007199254740992.0;
	}

private:
	void init(uint32 seed = 19650218UL) {
		state[0] =  seed & 4294967295UL;
		for(int j = 1; j < N; ++j) {
			state[j]  =  (1812433253UL * (state[j - 1] ^ (state[j - 1]  >>  30)) + j);
			// See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier.
			// In the previous versions, MSBs of the seed affect
			// only MSBs of the array state[].
			// 2002/01/09 modified by Makoto Matsumoto
			state[j] &=  4294967295UL;  // for >32 bit machines
		}
	}

	void next_state() {
		uint32* p = state;
		int i;

		for(i = N - M + 1; --i; ++p)
			*p = (p[M] ^ twist(p[0], p[1]));

		for(i = M; --i; ++p)
			*p = (p[M - N] ^ twist(p[0], p[1]));
		*p = p[M - N] ^ twist(p[0], state[0]);
		left = N;
		next = state;
	}

	uint32 mixbits(uint32 u, uint32 v) const {
		return (u & 2147483648UL) | (v & 2147483647UL);
	}

	uint32 twist(uint32 u, uint32 v) const {
		return ((mixbits(u, v)  >>  1) ^ (v & 1UL ? 2567483615UL : 0UL));
	}

	uint32 state[N];
	uint32 left;
	uint32* next;
};

/*
class mtrand_help {
	static mtrandom r;
public:
	mtrand_help() {}
	void operator()(uint32 s) { r.reset(s); }
	uint32 operator()() const { return r.rand(); }
	double operator()(double) { return r.real(); }
};
mtrandom mtrand_help:: r;

extern void mtsrand(uint32 s) { mtrand_help()(s); }
extern uint32 mtirand() { return mtrand_help()(); }
extern double mtdrand() { return mtrand_help()(1.0); }
*/

class Random {
public:
    Random( unsigned _seed = 0 ) {
        seed(_seed);
    }
    void seed( unsigned seed ) {
        r.reset( seed );
    }
    uint32 rand() {
        return r.rand();
    }
    int randint( uint32 maxnum ) {
        return int(randfloat() * maxnum);
    }
    double randfloat () {
        return r.real();
    }
private:
    mtrandom r;
};
// this is how tetrio generate new bag of minos
class RandomTetrio {
private:
	unsigned s;
public:
	RandomTetrio(unsigned seed = 0) {
		setSeed(seed);
	}
	void setSeed(unsigned seed) {
		s = seed % 2147483647;
		s = (s <= 0) ? s + 2147483646 : s;
	}
	unsigned next() {
		long long temp = s;
		s = (16807 * temp) % 2147483647;
		return s;
	}
	double nextFloat() {
		double tmp = next() - 1;
		return tmp / 2147483646.0;
	}
	void shuffleArray(int(&m)[7]) {
		int idx1, idx2 = 7;
		while (--idx2) {
			idx1 = nextFloat() * (idx2 + 1);
			std::swap(m[idx1], m[idx2]);
		}
		return;
	}
	unsigned getCurrentSeed() {
		return s;
	}
};
}
