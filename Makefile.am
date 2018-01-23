ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src data

bin_SCRIPTS = CSBwin-linux.sh

doc_DATA = AUTHORS CHANGES.TXT COPYING CSBWIN_LINUX_LIESMICH.TXT \
	CSBWIN_LINUX_README.TXT ChangeLog ConfigMac.txt README.linux ReleaseNotes.txt config.linux \
	config.txt i386.binzipname.txt i486.binzipname.txt ornate_floor_wall_types.txt srczipname.txt \
	x86_64.binzipname.txt

EXTRA_DIST = CSBwin-linux.sh.in $(doc_DATA)

DISTCLEANFILES = CSBwin-linux.sh

AM_DISTCHECK_CONFIGURE_FLAGS = --disable-esound --disable-dynwinsize

CSBwin-linux.sh: $(top_srcdir)/CSBwin-linux.sh.in
	$(AM_V_GEN)$(SED) \
	-e 's,@datadir[@],$(pkgdatadir),g' \
	-e 's,@prefix[@],$(prefix),g' \
	-e 's,@PACKAGE[@],$(PACKAGE),g' < $< > $@
