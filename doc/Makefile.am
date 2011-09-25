EXTRA_DIST = manuel.tex

doc : manuel.tex
	sh ./doc_gen_tex.sh
	pdflatex manuel.tex manuel.pdf
	bibtex manuel
# On le fait deux fois pour résoudre le problème du sommaire.
	pdflatex manuel.tex manuel.pdf
	pdflatex manuel.tex manuel.pdf

CLEANFILES = *.log *.aux manuel.pdf *.out *.tmp *.toc *.bbl *.blg
