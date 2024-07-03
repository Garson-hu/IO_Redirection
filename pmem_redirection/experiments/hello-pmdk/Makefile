CFLAGS = -std=gnu99 -Wall -Wextra -O3
LDLIBS = -lpmem -lpmemobj

EXE = hello-libpmem hello-libpmemobj
WORKFLOW = pmem-store pmem-print
POOL_WORKFLOW = pmem-pool-store pmem-pool-print

PMEM_FILE = pmem-file
PMEM_POOL_FILE = pmem-pool-file

WORKFLOW_FILE = numbers.txt

all: $(EXE) $(WORKFLOW) $(POOL_WORKFLOW)

test: all test_pmdk test_workflow test_pool_workflow

test_pmdk:
	$(foreach var,$(EXE),./$(var) &&) echo -n

test_workflow:
	rm -f $(PMEM_FILE)
	./pmem-store && ./pmem-print | tee $(WORKFLOW_FILE)
	./check.sh

test_pool_workflow:
	rm -f $(PMEM_POOL_FILE)
	./pmem-pool-store && ./pmem-pool-print | tee $(WORKFLOW_FILE)
	./check.sh

clean:
	$(RM) \
	$(EXE) \
	$(WORKFLOW) \
	$(POOL_WORKFLOW) \
	$(PMEM_FILE) \
	$(PMEM_POOL_FILE) \
	$(WORKFLOW_FILE)
