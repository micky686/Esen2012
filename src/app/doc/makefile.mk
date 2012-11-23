# Makefile fragment. Modulename is the directory name.
# (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>

# mandatory prelude
include $(MSTDPRE)

TEXDIR		:= $(d)/tex

CLEANFILE	+= $(TEXDIR)/*.aux $(d)/*.aux $(d)/*.toc $(d)/*.log $(d)/*.bbl $(d)/*.blg $(d)/*~ $(d)/ese_prot.pdf

labprot: $(d)/ese_prot.pdf

$(d)/ese_prot.pdf:  $(d)/ese_prot.tex $(wildcard $(TEXDIR)/*.tex)
	cd ./doc; pdflatex ese_prot.tex && pdflatex ese_prot.tex; cd .. 


spellcheck:
	LANG=${LANG} $(SCR_SPELLCHECK) ./doc/ese_prot.tex $(TEXDIR)/*.tex
	rm -f $(d)/*.bak
	rm -f $(TEXDIR)/*.bak

PHONY		+= spell labprot

#mandatory postlude
include $(MSTDPOST)
