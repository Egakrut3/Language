.PHONY : test front_end middle_end back_end

test : front_end middle_end
	@echo Full compilation finished

#TODO - very bad practice
#TODO - how to avoid -B option always
front_end :
	@make -B -C ./Front-end/
	@./Front-end/bin/Test.exe --in ./Code --out ./Front_end_tree

middle_end :
	@make -B -C ./Middle-end/
	@./Middle-end/bin/Test.exe --in ./Front_end_tree --out ./Middle_end_tree

back_end :
	@make -B -C ./Back-end/
	@./Back-end/bin/Test.exe --in ./Middle_end_tree --out ./Asm_code
