#pragma once

#include "Gogaman/Base.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	namespace RadixSort
	{
		//Least significant digit radix sort | Time complexity: O(5n) | Space complexity: O(n)
		uint32_t *Sort(const uint32_t numKeys, uint32_t *keys)
		{
			GM_ASSERT(numKeys > 0, "Failed to sort: invalid number of keys")
			GM_ASSERT(keys,        "Failed to sort: invalid keys")
			
			//Generate histograms
			uint32_t key                  = 0;
			uint32_t previousKey          = 0;
			uint32_t histograms[4][0x100] = { 0 };
			bool     sortRequired         = false;
			for(uint32_t i = 0; i < numKeys; i++)
			{
				key = keys[i];
				if(key < previousKey)
					sortRequired = true;

				previousKey = key;
				
				for(auto j = 0; j < 4; j++)
				{
					histograms[j][key & 0xff]++;
					key >>= 8;
				}
			}

			if(sortRequired)
			{
				uint32_t firstKey            = keys[0];
				uint8_t  unsortedRadixPlaces = 0;
				uint8_t  unsortedRadixPlaceIndices[4];
				for(auto i = 0; i < 4; i++)
					if(histograms[i][(firstKey >> (i << 3)) & 0xff] != numKeys)
						unsortedRadixPlaceIndices[unsortedRadixPlaces++] = i;

				//Generate prefix sums
				for(auto i = 0; i < unsortedRadixPlaces; i++)
				{
					uint32_t offset = 0;
					//TODO: Replace with j < 255 and do special case for j = 255 that doesn't increment counter
					for(auto j = 0; j < 0x100; j++)
					{
						uint32_t counter = histograms[unsortedRadixPlaceIndices[i]][j];
						histograms[unsortedRadixPlaceIndices[i]][j] = offset;
						offset += counter;
					}
				}

				//Permutation
				uint32_t *buffer = new uint32_t[numKeys];
				for(auto i = 0; i < unsortedRadixPlaces; i++)
				{
					for(uint32_t j = 0; j < numKeys; j++)
					{
						uint32_t key  = keys[j];
						buffer[histograms[unsortedRadixPlaceIndices[i]][(key >> (unsortedRadixPlaceIndices[i] << 3)) & 0xff]++] = key;
					}

					std::swap(keys, buffer);
				}

				std::cout << "Sorted " << +unsortedRadixPlaces << " radix places" << std::endl;

				delete[] buffer;
				return keys;
			}
			else
				return keys;
		}
	}
}