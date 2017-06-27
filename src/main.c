#include "../include/mem_struct.h"

void* my_malloc_hook(size_t size, const void* caller) {
	size_t free_mem = 0;
	void* result = NULL;
	char *ptr = NULL;
	MEM_CHUNK_STRUCT *mem_chunk = NULL, *m_curs = NULL, *s_curs = NULL;

	if (size == 0)
		return NULL;

	size = (size + 7) / 8;
	size *= 8;

	m_curs = mem_head->mp_next;
	while(m_curs != mem_tail){
		if (m_curs->stat == 0) {
			s_curs = m_curs->mp_next;
			free_mem += m_curs->size;

			while(free_mem < size) {
				if (s_curs->stat == 0)
					free_mem += s_curs->size + sizeof(MEM_CHUNK_STRUCT);
				else
					break;

				s_curs = s_curs->mp_next;
			}

			if (free_mem >= size)
				break;
			else
				m_curs = s_curs;
		}

		free_mem = 0;
		m_curs = m_curs->mp_next;
	}

	if (m_curs == mem_tail) {
		mem_chunk = sbrk(size + sizeof(MEM_CHUNK_STRUCT));
		result = mem_chunk + 1;

		mem_chunk->size = size;
		mem_chunk->stat = 1;

		if (mem_head->mp_next == mem_tail) {
			mem_chunk->mp_next = mem_tail;
			mem_chunk->mp_prev = mem_head;

			mem_head->mp_next = mem_chunk;
			mem_tail->mp_prev = mem_chunk;
		}
		else {
			mem_chunk->mp_next = mem_tail;
			mem_chunk->mp_prev = mem_tail->mp_prev;

			mem_tail->mp_prev->mp_next = mem_chunk;
			mem_tail->mp_prev = mem_chunk;
		}
	}
	else {
  		if (free_mem - size >= sizeof(MEM_CHUNK_STRUCT) + 16) {
			ptr = (char*) m_curs;

			ptr += size + sizeof(MEM_CHUNK_STRUCT);
			mem_chunk = (MEM_CHUNK_STRUCT*) ptr;

			mem_chunk->size = free_mem - size - sizeof(MEM_CHUNK_STRUCT);
			mem_chunk->stat = 0;

			mem_chunk->mp_next = s_curs;
			mem_chunk->mp_prev = m_curs;

			s_curs->mp_prev = mem_chunk;

			m_curs->size = size;
			m_curs->stat = 1;

			m_curs->mp_next = mem_chunk;
		}
		else {
			m_curs->size = free_mem;
			m_curs->stat = 1;

			m_curs->mp_next = s_curs;
			s_curs->mp_prev = m_curs;
		}

		result = ++m_curs;
	}

	hub = sbrk(0);

	printf("[Memory status]: malloc(%d) called from %p returns %p\n", /*(free_mem > 0) ? (int) free_mem :*/ (int) size, caller, result);
	return (errno != ENOMEM) ? result : NULL;
}

void my_free_hook(void* ptr, const void* caller) {
	size_t free_mem = 0;
	MEM_CHUNK_STRUCT *mem_chunk = NULL, *m_curs = NULL, *s_curs = NULL;

	printf("[In] Heap upper bound:  %p\n", hub);

	mem_chunk = (MEM_CHUNK_STRUCT*) ptr;
	mem_chunk--;

	if (mem_chunk->stat == 0)
		return;

	mem_chunk->stat = 0;

	m_curs = mem_chunk;
	while(m_curs != mem_tail) {
		if (m_curs->stat == 1)
			break;

		free_mem += m_curs->size + sizeof(MEM_CHUNK_STRUCT);
		m_curs = m_curs->mp_next;
	}

	if (m_curs == mem_tail) {
		m_curs = mem_chunk->mp_prev;
		while(m_curs != mem_head) {
			if (m_curs->stat == 1)
				break;

			free_mem += m_curs->size + sizeof(MEM_CHUNK_STRUCT);
			m_curs = m_curs->mp_prev;
		}

		mem_tail->mp_prev = m_curs;
		m_curs->mp_next = mem_tail;

		sbrk(0 - free_mem);
	}

	hub = sbrk(0);

	printf("[Out] Heap upper bound: %p\n", hub);
	/*printf("[Memory status]: free(ptr = %p) called from %p\n", ptr, caller);*/
}

void init(void){
	hlb = sbrk(0);
	mem_head = sbrk(sizeof(MEM_CHUNK_STRUCT));
	mem_tail = sbrk(sizeof(MEM_CHUNK_STRUCT));
	hub = sbrk(0);

	printf("\n[Start] Heap lower bound: %p\n", hlb);
	printf("[Start] Heap upper bound: %p\n\n", hub);

	mem_head->size = sizeof(MEM_CHUNK_STRUCT);
	mem_head->stat = 1;
	mem_head->mp_next = mem_tail;
	mem_head->mp_prev = NULL;

	mem_tail->size = sizeof(MEM_CHUNK_STRUCT);
	mem_tail->stat = 1;
	mem_tail->mp_next = NULL;
	mem_tail->mp_prev = mem_head;

	__malloc_hook = my_malloc_hook;
  	__free_hook = my_free_hook;
}

void (*__malloc_initialize_hook)(void) = init;

int main() {
	int i = 0, j = 0;
	int ** mem = NULL;

	printf("\nMalloc: mem\n");
	mem = malloc(3 * sizeof(int*));
	for (i = 0; i < 3; i++) {
		printf("\nMalloc: mem[%d]\n", i);
		mem[i] = malloc(10 * sizeof(int));
	}

	printf("\nFree: mem[1]\n");
	free(mem[1]);

	printf("\nMalloc: mem[1]\n");
	mem[1] = malloc(5 * sizeof(int));

	printf("\nFree: mem[1]\n");
	free(mem[1]);

	printf("\nFree: mem[0]\n");
	free(mem[0]);

	printf("\nFree: mem[2]\n");
	free(mem[2]);

	printf("\nFree: mem\n");
	free(mem);

	printf("[End] Heap lower bound: %p\n", hlb);
	printf("[End] Heap upper bound: %p\n\n", hub);

	return 0;
}
