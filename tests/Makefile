TEST_CHECKS_BUILD_LIST = $(subst Makefile,build_check, $(wildcard ./*/Makefile))

build_check: $(TEST_CHECKS_BUILD_LIST)

%/build_check:
	echo "=== build check for $(dir $@) ==="
	cd $(dir $@) ; $(MAKE) build_check ; cd -
