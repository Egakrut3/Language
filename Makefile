.PHONY : test front_end

test : front_end middle_end
	@echo Full compilation finished

#TODO - very bad practice
#TODO - how to avoid -B option always
front_end :
	@make -B -C ./Front-end/
	@./Front-end/bin/Test.exe --in ./Code

middle_end :
	@make -B -C ./Middle-end/
	@./Middle-end/bin/Test.exe --in ./Tree
