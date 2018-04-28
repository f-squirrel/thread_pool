TARGETS=ThreadPoolTest

CPP_FLAGS=-W -Wall -std=c++14 -stdlib=libc++ \
		  -L/usr/lib -lboost_thread-mt -lboost_system

all: $(TARGETS)

test: $(TARGETS) $(addprefix run-,$(TARGETS))

%: %.cpp
	$(CXX) $(CPP_FLAGS) -o $@ $^ -lboost_unit_test_framework

run-%: %
	-./$^ --output_format=HRF --log_level=test_suite

clean:
	rm -rf $(TARGETS)
