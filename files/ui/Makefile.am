UI_FILES = $(wildcard *.ui)
UI_H_FILES = $(UI_FILES:.ui=.ui.h)

all : $(UI_FILES)
	@echo '<?xml version="1.0" encoding="UTF-8"?><gresources><gresource prefix="/org/2lgc/codegui">' > ressources.xml
	$(MAKE) uifiles
	@echo '</gresource></gresources>' >> ressources.xml

uifiles : $(UI_H_FILES)

%.ui.h : %.ui
	@echo "<file compressed=\"true\">$^</file>" >> ressources.xml

CLEANFILES = *.ui.h ressources.xml
