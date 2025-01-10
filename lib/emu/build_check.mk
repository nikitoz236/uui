
%/folder_build_check:
	echo "\n\n\n=== build check for $(dir $@) ===\n"
	cd $(dir $@) ; $(MAKE) build_check ; cd -

%/folder_hw_build_check:
	echo "\n\n\n=== build check for $(dir $@) ===\n"
	cd $(dir $@) ; $(MAKE) ; $(MAKE) clean ; cd -
