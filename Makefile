.PHONY: clean All

All:
	@echo "----------Building project:[ libhdbpp-elk - Debug ]----------"
	$(MAKE) -C "." -f "libhdbpp-elk.mk"
clean:
	@echo "----------Cleaning project:[ libhdbpp-elk - Debug ]----------"
	$(MAKE) -C "." -f "libhdbpp-elk.mk" clean
