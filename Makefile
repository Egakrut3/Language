.PHONY : test prepare front_end middle_end back_end

test : prepare front_end middle_end
	@echo Full compilation finished

#TODO - how to avoid -B option always
prepare :
	@make -B -C ./VLVR/My_functions/
	@make -B -C ./VLVR/Expression_tree/
	@make -B -C ./VLVR/Name_table/

#TODO - very bad practice
front_end : prepare
	@make -B -C ./Front_end/
	@./Front_end/bin/Front_end.exe --in Code --out Front_end_tree

middle_end : prepare
	@make -B -C ./Middle_end/
	@./Middle_end/bin/Middle_end.exe --in Front_end_tree --out Middle_end_tree

back_end : prepare
	@make -B -C ./Back_end/
	@./Back_end/bin/Back_end.exe --in Middle_end_tree --out Asm_code
