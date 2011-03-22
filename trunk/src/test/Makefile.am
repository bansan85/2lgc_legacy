DEFAULT_INCLUDES	=	-I../.. \
				-I../lib/1990 \
				-I../lib/common

TESTS	=	1990__1_actions \
		1990__1_coef_psi \
		1990__1_combinaisons_groupes \
		1990__1_duree

1990__1_actions_SOURCES	=	1990__1_actions.c
1990__1_actions_LDADD	=	../lib/1990/lib1990.la ../lib/common/libcommon.la

1990__1_coef_psi_SOURCES	=	1990__1_coef_psi.c
1990__1_coef_psi_LDADD		=	../lib/1990/lib1990.la ../lib/common/libcommon.la

1990__1_combinaisons_groupes_SOURCES	=	1990__1_combinaisons_groupes.c
1990__1_combinaisons_groupes_LDADD	=	../lib/1990/lib1990.la ../lib/common/libcommon.la

1990__1_duree_SOURCES	=	1990__1_duree.c
1990__1_duree_LDADD	=	../lib/1990/lib1990.la ../lib/common/libcommon.la

EXTRA_PROGRAMS = $(TESTS)
