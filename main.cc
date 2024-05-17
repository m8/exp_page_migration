#include <iostream>
#include <unistd.h>
#include <numa.h>
#include <vector>
#include <chrono>
#include <numaif.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096
#define MB (1024 * 1024UL)
#define GB (1024 * MB)
#define MPOL_F_STATIC_NODES	(1 << 15)
#define MPOL_F_RELATIVE_NODES	(1 << 14)
#define NUMA_NODES_AVAILABLE 4

#ifdef HUGE_PAGES
#undef PAGE_SIZE
#define PAGE_SIZE (2*MB)
#endif

#define HUGE_PAGES

void bench(size_t data_size, int dest_node)
{	
	auto page_count = data_size / PAGE_SIZE;
	
	auto pages = (char*)malloc(PAGE_SIZE * (page_count+1));
	auto addr = (void**)malloc(sizeof(char *) * page_count);
	auto status = (int*)malloc(sizeof(int) * page_count);
	auto nodes = (int*)malloc(sizeof(int) * page_count);

	// Flag
	int flag = 0;

	#ifdef HUGE_PAGES
	madvise(pages, PAGE_SIZE * page_count, MADV_HUGEPAGE);
	#endif

	// Force alloc
	for (size_t t = 0; t < data_size; t += PAGE_SIZE) {pages[t] = 0;};

	for (size_t i = 0; i < page_count; i++) {
		addr[i] = pages + i * PAGE_SIZE;
		nodes[i] = dest_node;
		status[i] = 0;
	}
    auto start = std::chrono::high_resolution_clock::now();
	
	int rc = numa_move_pages(0, page_count, addr, nodes, status, flag);
	if (rc != 0){
		perror("move 1");
	}
    auto end = std::chrono::high_resolution_clock::now();
    
	std::chrono::duration<double> duration = end - start;

	double time_taken = duration.count();  
    double throughput = data_size / time_taken; 

    std::cout << data_size/MB << "MB " << time_taken << " seconds, " << throughput / (1024 * 1024) << " MB/second" << std::endl;
	free(pages);
	free(addr);
	free(status);
	free(nodes);
}


int main(int argc, char *argv[])
{
	int alloc_node = 0;
	int dest_node = 2;

	if (argc >= 1) { alloc_node = atoi(argv[1]); }	
	if (argc >= 2) { dest_node = atoi(argv[2]); }

	unsigned long nodemask = 1ul << alloc_node;
    set_mempolicy(MPOL_BIND | MPOL_F_STATIC_NODES, &nodemask, NUMA_NODES_AVAILABLE + 1);

    std::vector<size_t> sizes = {1*MB, 64*MB, 256*MB, 1*GB, 4*GB, 16*GB, 64*GB};
	// std::vector<size_t> sizes = {1*MB, 64*MB, 256*MB, 1*GB};

	for(auto size: sizes){
		bench(size, dest_node);
	}
	return 0;
}