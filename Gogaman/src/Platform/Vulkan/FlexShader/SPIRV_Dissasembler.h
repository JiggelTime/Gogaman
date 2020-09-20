#pragma once

struct sv_instr {
	uint16_t word_count;
	uint16_t opcode;
	uint32_t *operands;
};

struct sv_module {
	uint32_t magic;
	uint32_t version;
	uint32_t generator;
	uint32_t bound;
	uint32_t reserved;

	struct sv_instr *instrs;
	int num_instrs;
};

#define SV_MAGIC 0x07230203
#define SV_MAGIC_REV 0x03022307

inline int sv_read_instr(struct sv_instr *dst, uint32_t **words, int *num_words)
{
	if(*num_words < 1)
		return -1;

	dst->word_count = **words >> 16;
	dst->opcode = **words & 0xFFFF;

	/*
		 * WordCount needs to be at least 1, to encode the WordCount and
	 * Opcode itself
		 */
	if(dst->word_count < 1)
		return -1; /* malformed instruction */

	if(dst->word_count > *num_words)
		return -1; /* not enough data */

	dst->operands = *words;

	*words += dst->word_count;
	*num_words -= dst->word_count;
	return 0;
}

inline int sv_read(struct sv_module *dst, uint32_t *words, int num_words)
{
	/* check if we have enough data to read header */
	if(num_words < 5)
		return -1;

	dst->magic = *words++;
	dst->version = *words++;
	dst->generator = *words++;
	dst->bound = *words++;
	dst->reserved = *words++;

	/* verify header-data */
	//if(dst->magic != SV_MAGIC || dst->version != 99)
	if(dst->magic != SV_MAGIC || dst->version != 0x00010000)
		return -1;

	num_words -= 5;

	dst->instrs = NULL;
	dst->num_instrs = 0;
	while(num_words > 0) {
		struct sv_instr instr;
		if(sv_read_instr(&instr, &words, &num_words) < 0)
			return -1;

		dst->instrs = (sv_instr *)realloc(dst->instrs, (dst->num_instrs + 1) * sizeof(instr));
		if(!dst->instrs)
			return -1;

		dst->instrs[dst->num_instrs++] = instr;
	}

	return 0;
}

inline uint32_t bswap_32(uint32_t x)
{
	return ((x & 0xff000000) >> 24) | ((x & 0x00ff0000) >> 8) | \
		((x & 0x0000ff00) << 8) | ((x & 0x000000ff) << 24);
}

inline int printSPIRV(uint32_t numWords, uint32_t *wordz)
{
	struct sv_module mod;
	uint32_t word, *words = NULL;
	int num_words = 0, i;

	#if 0
	while(fread(&word, 4, 1, stdin) == 1) {
		words = (uint32_t *)realloc(words, (num_words + 1) * 4);
		if(!words) {
			perror("realloc");
			abort();
		}
		words[num_words++] = word;
	}
	#endif

	#if 1
	num_words = numWords;
	words = wordz;
	#endif

	/* byte-swap input if needed */
	if(num_words > 0 && words[0] == SV_MAGIC_REV)
		for(i = 0; i < num_words; ++i)
			words[i] = bswap_32(words[i]);

	printf("words: %d\n", num_words);

	if(sv_read(&mod, words, num_words) < 0) {
		fprintf(stderr, "failed to read SPIR-V module\n");
		return 1;
	}

	for(i = 0; i < mod.num_instrs; ++i)
	{
		printf("OpCode: %d | ", mod.instrs[i].opcode);
		for(uint32_t j = 1; j < mod.instrs[i].word_count; j++)
		{
			printf("Operand %d: %d | ", j, mod.instrs[i].operands[j]);
		}

		printf("\n");
	}

	return 0;
}