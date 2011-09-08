EXTRA_DIST = manuel.tex

doc : manuel.tex
	sh ./doc_gen_tex.sh
	pdflatex manuel.tex manuel.pdf
# On le fait deux fois pour résoudre le problème du sommaire.
	pdflatex manuel.tex manuel.pdf

CLEANFILES = *.log *.aux *pdf *.out *.tmp *.toc
