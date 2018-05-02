TARGETS=ThreadPoolTest

CPP_FLAGS=-W -Wall \
		  -std=c++14 \
		  -L/usr/lib \
		  -pthread
ifeq ($(HAS_LIBCXX11), 1)
	CPP_FLAGS += -stdlib=libc++
endif

all: $(TARGETS)

test: $(TARGETS) $(addprefix run-,$(TARGETS))

%: %.cpp
	$(CXX) $(CPP_FLAGS) -o $@ $^ -lboost_unit_test_framework

run-%: %
	-./$^ --output_format=HRF --log_level=test_suite

clean:
	rm -rf $(TARGETS)
