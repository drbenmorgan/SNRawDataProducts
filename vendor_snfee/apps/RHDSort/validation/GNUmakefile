.PHONY: all
all: snfee-rhdsort_validation.pdf

.PHONY: show
show: snfee-rhdsort_validation.pdf
	xdg-open $< &

snfee-rhdsort_validation.pdf : README.rst
	rst2pdf $<
	mv README.pdf $@

.PHONY: clean
clean:
	$(RM) README.pdf 
	$(RM) snfee-rhdsort_validation 

# end
