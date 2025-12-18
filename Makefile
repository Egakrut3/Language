SUB_DIRS = ./Front_end/ ./Middle_end/ ./Back_end/

.PHONY: test prepare clean Front_end Middle_end Back_end

test:
	@$(MAKE) Front_end
	@$(MAKE) Middle_end
	@$(MAKE) Back_end
	@echo Full compilation finished

prepare:
	@$(MAKE) -C ./VLVR/

Front_end: | prepare
	@$(MAKE) -C ./Front_end/
	@./Front_end/bin/Front_end.exe --in Code --out Front_end_tree

Middle_end: | prepare
	@$(MAKE) -C ./Middle_end/
	@./Middle_end/bin/Middle_end.exe --in Front_end_tree --out Middle_end_tree

Back_end: | prepare
	@$(MAKE) -C ./Back_end/
	@./Back_end/bin/Back_end.exe --in Middle_end_tree --out Asm_code

clean:
	@$(MAKE) clean -C ./VLVR/

	@for dir in $(SUB_DIRS); do	\
		$(MAKE) clean -C $$dir;	\
	done
