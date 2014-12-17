
#ifndef __MIME_TYPES_H__
#define __MIME_TYPES_H__

enum {
	/* application/andrew-inset */
	APPLICATION_ANDREW_INSET,
	/* application/annodex */
	APPLICATION_ANNODEX,
	/* application/atom+xml */
	APPLICATION_ATOM_XML,
	/* application/dicom */
	APPLICATION_DICOM,
	/* application/ecmascript */
	APPLICATION_ECMASCRIPT,
	/* application/epub+zip */
	APPLICATION_EPUB_ZIP,
	/* application/font-woff */
	APPLICATION_FONT_WOFF,
	/* application/gml+xml */
	APPLICATION_GML_XML,
	/* application/gnunet-directory */
	APPLICATION_GNUNET_DIRECTORY,
	/* application/gzip */
	APPLICATION_GZIP,
	/* application/illustrator */
	APPLICATION_ILLUSTRATOR,
	/* application/javascript */
	APPLICATION_JAVASCRIPT,
	/* application/mac-binhex40 */
	APPLICATION_MAC_BINHEX40,
	/* application/mathematica */
	APPLICATION_MATHEMATICA,
	/* application/mathml+xml */
	APPLICATION_MATHML_XML,
	/* application/mbox */
	APPLICATION_MBOX,
	/* application/metalink+xml */
	APPLICATION_METALINK_XML,
	/* application/metalink4+xml */
	APPLICATION_METALINK4_XML,
	/* application/msword */
	APPLICATION_MSWORD,
	/* application/msword-template */
	APPLICATION_MSWORD_TEMPLATE,
	/* application/mxf */
	APPLICATION_MXF,
	/* application/octet-stream */
	APPLICATION_OCTET_STREAM,
	/* application/oda */
	APPLICATION_ODA,
	/* application/ogg */
	APPLICATION_OGG,
	/* application/oxps */
	APPLICATION_OXPS,
	/* application/pdf */
	APPLICATION_PDF,
	/* application/pgp-encrypted */
	APPLICATION_PGP_ENCRYPTED,
	/* application/pgp-keys */
	APPLICATION_PGP_KEYS,
	/* application/pgp-signature */
	APPLICATION_PGP_SIGNATURE,
	/* application/pkcs10 */
	APPLICATION_PKCS10,
	/* application/pkcs10+pem */
	APPLICATION_PKCS10_PEM,
	/* application/pkcs12+pem */
	APPLICATION_PKCS12_PEM,
	/* application/pkcs7-mime */
	APPLICATION_PKCS7_MIME,
	/* application/pkcs7-mime+pem */
	APPLICATION_PKCS7_MIME_PEM,
	/* application/pkcs7-signature */
	APPLICATION_PKCS7_SIGNATURE,
	/* application/pkcs8 */
	APPLICATION_PKCS8,
	/* application/pkcs8+pem */
	APPLICATION_PKCS8_PEM,
	/* application/pkix-cert */
	APPLICATION_PKIX_CERT,
	/* application/pkix-cert+pem */
	APPLICATION_PKIX_CERT_PEM,
	/* application/pkix-crl */
	APPLICATION_PKIX_CRL,
	/* application/pkix-crl+pem */
	APPLICATION_PKIX_CRL_PEM,
	/* application/pkix-pkipath */
	APPLICATION_PKIX_PKIPATH,
	/* application/postscript */
	APPLICATION_POSTSCRIPT,
	/* application/prs.plucker */
	APPLICATION_PRS_PLUCKER,
	/* application/ram */
	APPLICATION_RAM,
	/* application/rdf+xml */
	APPLICATION_RDF_XML,
	/* application/relax-ng-compact-syntax */
	APPLICATION_RELAX_NG_COMPACT_SYNTAX,
	/* application/rss+xml */
	APPLICATION_RSS_XML,
	/* application/rtf */
	APPLICATION_RTF,
	/* application/sdp */
	APPLICATION_SDP,
	/* application/sieve */
	APPLICATION_SIEVE,
	/* application/smil */
	APPLICATION_SMIL,
	/* application/sql */
	APPLICATION_SQL,
	/* application/vnd.adobe.flash.movie */
	APPLICATION_VND_ADOBE_FLASH_MOVIE,
	/* application/vnd.android.package-archive */
	APPLICATION_VND_ANDROID_PACKAGE_ARCHIVE,
	/* application/vnd.apple.mpegurl */
	APPLICATION_VND_APPLE_MPEGURL,
	/* application/vnd.corel-draw */
	APPLICATION_VND_COREL_DRAW,
	/* application/vnd.emusic-emusic_package */
	APPLICATION_VND_EMUSIC_EMUSIC_PACKAGE,
	/* application/vnd.google-earth.kml+xml */
	APPLICATION_VND_GOOGLE_EARTH_KML_XML,
	/* application/vnd.google-earth.kmz */
	APPLICATION_VND_GOOGLE_EARTH_KMZ,
	/* application/vnd.hp-hpgl */
	APPLICATION_VND_HP_HPGL,
	/* application/vnd.hp-pcl */
	APPLICATION_VND_HP_PCL,
	/* application/vnd.iccprofile */
	APPLICATION_VND_ICCPROFILE,
	/* application/vnd.lotus-1-2-3 */
	APPLICATION_VND_LOTUS_1_2_3,
	/* application/vnd.lotus-wordpro */
	APPLICATION_VND_LOTUS_WORDPRO,
	/* application/vnd.mozilla.xul+xml */
	APPLICATION_VND_MOZILLA_XUL_XML,
	/* application/vnd.ms-access */
	APPLICATION_VND_MS_ACCESS,
	/* application/vnd.ms-cab-compressed */
	APPLICATION_VND_MS_CAB_COMPRESSED,
	/* application/vnd.ms-excel */
	APPLICATION_VND_MS_EXCEL,
	/* application/vnd.ms-excel.addin.macroEnabled.12 */
	APPLICATION_VND_MS_EXCEL_ADDIN_MACROENABLED_12,
	/* application/vnd.ms-excel.sheet.binary.macroEnabled.12 */
	APPLICATION_VND_MS_EXCEL_SHEET_BINARY_MACROENABLED_12,
	/* application/vnd.ms-excel.sheet.macroEnabled.12 */
	APPLICATION_VND_MS_EXCEL_SHEET_MACROENABLED_12,
	/* application/vnd.ms-excel.template.macroEnabled.12 */
	APPLICATION_VND_MS_EXCEL_TEMPLATE_MACROENABLED_12,
	/* application/vnd.ms-htmlhelp */
	APPLICATION_VND_MS_HTMLHELP,
	/* application/vnd.ms-powerpoint */
	APPLICATION_VND_MS_POWERPOINT,
	/* application/vnd.ms-powerpoint.addin.macroEnabled.12 */
	APPLICATION_VND_MS_POWERPOINT_ADDIN_MACROENABLED_12,
	/* application/vnd.ms-powerpoint.presentation.macroEnabled.12 */
	APPLICATION_VND_MS_POWERPOINT_PRESENTATION_MACROENABLED_12,
	/* application/vnd.ms-powerpoint.slide.macroEnabled.12 */
	APPLICATION_VND_MS_POWERPOINT_SLIDE_MACROENABLED_12,
	/* application/vnd.ms-powerpoint.slideshow.macroEnabled.12 */
	APPLICATION_VND_MS_POWERPOINT_SLIDESHOW_MACROENABLED_12,
	/* application/vnd.ms-powerpoint.template.macroEnabled.12 */
	APPLICATION_VND_MS_POWERPOINT_TEMPLATE_MACROENABLED_12,
	/* application/vnd.ms-publisher */
	APPLICATION_VND_MS_PUBLISHER,
	/* application/vnd.ms-tnef */
	APPLICATION_VND_MS_TNEF,
	/* application/vnd.ms-word */
	APPLICATION_VND_MS_WORD,
	/* application/vnd.ms-word.document.macroEnabled.12 */
	APPLICATION_VND_MS_WORD_DOCUMENT_MACROENABLED_12,
	/* application/vnd.ms-word.template.macroEnabled.12 */
	APPLICATION_VND_MS_WORD_TEMPLATE_MACROENABLED_12,
	/* application/vnd.ms-works */
	APPLICATION_VND_MS_WORKS,
	/* application/vnd.ms-wpl */
	APPLICATION_VND_MS_WPL,
	/* application/vnd.nintendo.snes.rom */
	APPLICATION_VND_NINTENDO_SNES_ROM,
	/* application/vnd.oasis.opendocument.chart */
	APPLICATION_VND_OASIS_OPENDOCUMENT_CHART,
	/* application/vnd.oasis.opendocument.chart-template */
	APPLICATION_VND_OASIS_OPENDOCUMENT_CHART_TEMPLATE,
	/* application/vnd.oasis.opendocument.database */
	APPLICATION_VND_OASIS_OPENDOCUMENT_DATABASE,
	/* application/vnd.oasis.opendocument.formula */
	APPLICATION_VND_OASIS_OPENDOCUMENT_FORMULA,
	/* application/vnd.oasis.opendocument.formula-template */
	APPLICATION_VND_OASIS_OPENDOCUMENT_FORMULA_TEMPLATE,
	/* application/vnd.oasis.opendocument.graphics */
	APPLICATION_VND_OASIS_OPENDOCUMENT_GRAPHICS,
	/* application/vnd.oasis.opendocument.graphics-flat-xml */
	APPLICATION_VND_OASIS_OPENDOCUMENT_GRAPHICS_FLAT_XML,
	/* application/vnd.oasis.opendocument.graphics-template */
	APPLICATION_VND_OASIS_OPENDOCUMENT_GRAPHICS_TEMPLATE,
	/* application/vnd.oasis.opendocument.image */
	APPLICATION_VND_OASIS_OPENDOCUMENT_IMAGE,
	/* application/vnd.oasis.opendocument.presentation */
	APPLICATION_VND_OASIS_OPENDOCUMENT_PRESENTATION,
	/* application/vnd.oasis.opendocument.presentation-flat-xml */
	APPLICATION_VND_OASIS_OPENDOCUMENT_PRESENTATION_FLAT_XML,
	/* application/vnd.oasis.opendocument.presentation-template */
	APPLICATION_VND_OASIS_OPENDOCUMENT_PRESENTATION_TEMPLATE,
	/* application/vnd.oasis.opendocument.spreadsheet */
	APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET,
	/* application/vnd.oasis.opendocument.spreadsheet-flat-xml */
	APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET_FLAT_XML,
	/* application/vnd.oasis.opendocument.spreadsheet-template */
	APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET_TEMPLATE,
	/* application/vnd.oasis.opendocument.text */
	APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT,
	/* application/vnd.oasis.opendocument.text-flat-xml */
	APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT_FLAT_XML,
	/* application/vnd.oasis.opendocument.text-master */
	APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT_MASTER,
	/* application/vnd.oasis.opendocument.text-template */
	APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT_TEMPLATE,
	/* application/vnd.oasis.opendocument.text-web */
	APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT_WEB,
	/* application/vnd.openofficeorg.extension */
	APPLICATION_VND_OPENOFFICEORG_EXTENSION,
	/* application/vnd.openxmlformats-officedocument.presentationml.presentation */
	APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION,
	/* application/vnd.openxmlformats-officedocument.presentationml.slide */
	APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_SLIDE,
	/* application/vnd.openxmlformats-officedocument.presentationml.slideshow */
	APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_SLIDESHOW,
	/* application/vnd.openxmlformats-officedocument.presentationml.template */
	APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_TEMPLATE,
	/* application/vnd.openxmlformats-officedocument.spreadsheetml.sheet */
	APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET,
	/* application/vnd.openxmlformats-officedocument.spreadsheetml.template */
	APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_TEMPLATE,
	/* application/vnd.openxmlformats-officedocument.wordprocessingml.document */
	APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT,
	/* application/vnd.openxmlformats-officedocument.wordprocessingml.template */
	APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_TEMPLATE,
	/* application/vnd.palm */
	APPLICATION_VND_PALM,
	/* application/vnd.rn-realmedia */
	APPLICATION_VND_RN_REALMEDIA,
	/* application/vnd.scribus */
	APPLICATION_VND_SCRIBUS,
	/* application/vnd.stardivision.calc */
	APPLICATION_VND_STARDIVISION_CALC,
	/* application/vnd.stardivision.chart */
	APPLICATION_VND_STARDIVISION_CHART,
	/* application/vnd.stardivision.draw */
	APPLICATION_VND_STARDIVISION_DRAW,
	/* application/vnd.stardivision.impress */
	APPLICATION_VND_STARDIVISION_IMPRESS,
	/* application/vnd.stardivision.mail */
	APPLICATION_VND_STARDIVISION_MAIL,
	/* application/vnd.stardivision.math */
	APPLICATION_VND_STARDIVISION_MATH,
	/* application/vnd.stardivision.writer */
	APPLICATION_VND_STARDIVISION_WRITER,
	/* application/vnd.sun.xml.base */
	APPLICATION_VND_SUN_XML_BASE,
	/* application/vnd.sun.xml.calc */
	APPLICATION_VND_SUN_XML_CALC,
	/* application/vnd.sun.xml.calc.template */
	APPLICATION_VND_SUN_XML_CALC_TEMPLATE,
	/* application/vnd.sun.xml.draw */
	APPLICATION_VND_SUN_XML_DRAW,
	/* application/vnd.sun.xml.draw.template */
	APPLICATION_VND_SUN_XML_DRAW_TEMPLATE,
	/* application/vnd.sun.xml.impress */
	APPLICATION_VND_SUN_XML_IMPRESS,
	/* application/vnd.sun.xml.impress.template */
	APPLICATION_VND_SUN_XML_IMPRESS_TEMPLATE,
	/* application/vnd.sun.xml.math */
	APPLICATION_VND_SUN_XML_MATH,
	/* application/vnd.sun.xml.writer */
	APPLICATION_VND_SUN_XML_WRITER,
	/* application/vnd.sun.xml.writer.global */
	APPLICATION_VND_SUN_XML_WRITER_GLOBAL,
	/* application/vnd.sun.xml.writer.template */
	APPLICATION_VND_SUN_XML_WRITER_TEMPLATE,
	/* application/vnd.symbian.install */
	APPLICATION_VND_SYMBIAN_INSTALL,
	/* application/vnd.tcpdump.pcap */
	APPLICATION_VND_TCPDUMP_PCAP,
	/* application/vnd.visio */
	APPLICATION_VND_VISIO,
	/* application/vnd.wordperfect */
	APPLICATION_VND_WORDPERFECT,
	/* application/winhlp */
	APPLICATION_WINHLP,
	/* application/x-5view */
	APPLICATION_X_5VIEW,
	/* application/x-7z-compressed */
	APPLICATION_X_7Z_COMPRESSED,
	/* application/x-abiword */
	APPLICATION_X_ABIWORD,
	/* application/x-ace */
	APPLICATION_X_ACE,
	/* application/x-alz */
	APPLICATION_X_ALZ,
	/* application/x-amipro */
	APPLICATION_X_AMIPRO,
	/* application/x-aportisdoc */
	APPLICATION_X_APORTISDOC,
	/* application/x-apple-diskimage */
	APPLICATION_X_APPLE_DISKIMAGE,
	/* application/x-applix-spreadsheet */
	APPLICATION_X_APPLIX_SPREADSHEET,
	/* application/x-applix-word */
	APPLICATION_X_APPLIX_WORD,
	/* application/x-arc */
	APPLICATION_X_ARC,
	/* application/x-archive */
	APPLICATION_X_ARCHIVE,
	/* application/x-arj */
	APPLICATION_X_ARJ,
	/* application/x-asp */
	APPLICATION_X_ASP,
	/* application/x-awk */
	APPLICATION_X_AWK,
	/* application/x-bcpio */
	APPLICATION_X_BCPIO,
	/* application/x-bittorrent */
	APPLICATION_X_BITTORRENT,
	/* application/x-blender */
	APPLICATION_X_BLENDER,
	/* application/x-brasero */
	APPLICATION_X_BRASERO,
	/* application/x-bzdvi */
	APPLICATION_X_BZDVI,
	/* application/x-bzip */
	APPLICATION_X_BZIP,
	/* application/x-bzip-compressed-tar */
	APPLICATION_X_BZIP_COMPRESSED_TAR,
	/* application/x-bzpdf */
	APPLICATION_X_BZPDF,
	/* application/x-bzpostscript */
	APPLICATION_X_BZPOSTSCRIPT,
	/* application/x-cb7 */
	APPLICATION_X_CB7,
	/* application/x-cbr */
	APPLICATION_X_CBR,
	/* application/x-cbt */
	APPLICATION_X_CBT,
	/* application/x-cbz */
	APPLICATION_X_CBZ,
	/* application/x-ccmx */
	APPLICATION_X_CCMX,
	/* application/x-cd-image */
	APPLICATION_X_CD_IMAGE,
	/* application/x-cdrdao-toc */
	APPLICATION_X_CDRDAO_TOC,
	/* application/x-chess-pgn */
	APPLICATION_X_CHESS_PGN,
	/* application/x-cisco-vpn-settings */
	APPLICATION_X_CISCO_VPN_SETTINGS,
	/* application/x-class-file */
	APPLICATION_X_CLASS_FILE,
	/* application/x-compress */
	APPLICATION_X_COMPRESS,
	/* application/x-compressed-tar */
	APPLICATION_X_COMPRESSED_TAR,
	/* application/x-core */
	APPLICATION_X_CORE,
	/* application/x-cpio */
	APPLICATION_X_CPIO,
	/* application/x-cpio-compressed */
	APPLICATION_X_CPIO_COMPRESSED,
	/* application/x-csh */
	APPLICATION_X_CSH,
	/* application/x-cue */
	APPLICATION_X_CUE,
	/* application/x-dar */
	APPLICATION_X_DAR,
	/* application/x-dbf */
	APPLICATION_X_DBF,
	/* application/x-dc-rom */
	APPLICATION_X_DC_ROM,
	/* application/x-deb */
	APPLICATION_X_DEB,
	/* application/x-designer */
	APPLICATION_X_DESIGNER,
	/* application/x-desktop */
	APPLICATION_X_DESKTOP,
	/* application/x-dia-diagram */
	APPLICATION_X_DIA_DIAGRAM,
	/* application/x-dia-shape */
	APPLICATION_X_DIA_SHAPE,
	/* application/x-docbook+xml */
	APPLICATION_X_DOCBOOK_XML,
	/* application/x-dvi */
	APPLICATION_X_DVI,
	/* application/x-e-theme */
	APPLICATION_X_E_THEME,
	/* application/x-egon */
	APPLICATION_X_EGON,
	/* application/x-etherpeek */
	APPLICATION_X_ETHERPEEK,
	/* application/x-executable */
	APPLICATION_X_EXECUTABLE,
	/* application/x-fictionbook+xml */
	APPLICATION_X_FICTIONBOOK_XML,
	/* application/x-fluid */
	APPLICATION_X_FLUID,
	/* application/x-font-afm */
	APPLICATION_X_FONT_AFM,
	/* application/x-font-bdf */
	APPLICATION_X_FONT_BDF,
	/* application/x-font-dos */
	APPLICATION_X_FONT_DOS,
	/* application/x-font-framemaker */
	APPLICATION_X_FONT_FRAMEMAKER,
	/* application/x-font-libgrx */
	APPLICATION_X_FONT_LIBGRX,
	/* application/x-font-linux-psf */
	APPLICATION_X_FONT_LINUX_PSF,
	/* application/x-font-otf */
	APPLICATION_X_FONT_OTF,
	/* application/x-font-pcf */
	APPLICATION_X_FONT_PCF,
	/* application/x-font-speedo */
	APPLICATION_X_FONT_SPEEDO,
	/* application/x-font-sunos-news */
	APPLICATION_X_FONT_SUNOS_NEWS,
	/* application/x-font-tex */
	APPLICATION_X_FONT_TEX,
	/* application/x-font-tex-tfm */
	APPLICATION_X_FONT_TEX_TFM,
	/* application/x-font-ttf */
	APPLICATION_X_FONT_TTF,
	/* application/x-font-ttx */
	APPLICATION_X_FONT_TTX,
	/* application/x-font-type1 */
	APPLICATION_X_FONT_TYPE1,
	/* application/x-font-vfont */
	APPLICATION_X_FONT_VFONT,
	/* application/x-frame */
	APPLICATION_X_FRAME,
	/* application/x-gameboy-rom */
	APPLICATION_X_GAMEBOY_ROM,
	/* application/x-gba-rom */
	APPLICATION_X_GBA_ROM,
	/* application/x-gdbm */
	APPLICATION_X_GDBM,
	/* application/x-gedcom */
	APPLICATION_X_GEDCOM,
	/* application/x-genesis-rom */
	APPLICATION_X_GENESIS_ROM,
	/* application/x-gettext-translation */
	APPLICATION_X_GETTEXT_TRANSLATION,
	/* application/x-glade */
	APPLICATION_X_GLADE,
	/* application/x-gmc-link */
	APPLICATION_X_GMC_LINK,
	/* application/x-gnome-saved-search */
	APPLICATION_X_GNOME_SAVED_SEARCH,
	/* application/x-gnucash */
	APPLICATION_X_GNUCASH,
	/* application/x-gnumeric */
	APPLICATION_X_GNUMERIC,
	/* application/x-gnuplot */
	APPLICATION_X_GNUPLOT,
	/* application/x-go-sgf */
	APPLICATION_X_GO_SGF,
	/* application/x-graphite */
	APPLICATION_X_GRAPHITE,
	/* application/x-gtk-builder */
	APPLICATION_X_GTK_BUILDER,
	/* application/x-gtktalog */
	APPLICATION_X_GTKTALOG,
	/* application/x-gz-font-linux-psf */
	APPLICATION_X_GZ_FONT_LINUX_PSF,
	/* application/x-gzdvi */
	APPLICATION_X_GZDVI,
	/* application/x-gzpdf */
	APPLICATION_X_GZPDF,
	/* application/x-gzpostscript */
	APPLICATION_X_GZPOSTSCRIPT,
	/* application/x-hdf */
	APPLICATION_X_HDF,
	/* application/x-hwp */
	APPLICATION_X_HWP,
	/* application/x-hwt */
	APPLICATION_X_HWT,
	/* application/x-ica */
	APPLICATION_X_ICA,
	/* application/x-iff */
	APPLICATION_X_IFF,
	/* application/x-ipod-firmware */
	APPLICATION_X_IPOD_FIRMWARE,
	/* application/x-iptrace */
	APPLICATION_X_IPTRACE,
	/* application/x-it87 */
	APPLICATION_X_IT87,
	/* application/x-java */
	APPLICATION_X_JAVA,
	/* application/x-java-archive */
	APPLICATION_X_JAVA_ARCHIVE,
	/* application/x-java-jce-keystore */
	APPLICATION_X_JAVA_JCE_KEYSTORE,
	/* application/x-java-jnlp-file */
	APPLICATION_X_JAVA_JNLP_FILE,
	/* application/x-java-keystore */
	APPLICATION_X_JAVA_KEYSTORE,
	/* application/x-java-pack200 */
	APPLICATION_X_JAVA_PACK200,
	/* application/x-jbuilder-project */
	APPLICATION_X_JBUILDER_PROJECT,
	/* application/x-karbon */
	APPLICATION_X_KARBON,
	/* application/x-kchart */
	APPLICATION_X_KCHART,
	/* application/x-kexi-connectiondata */
	APPLICATION_X_KEXI_CONNECTIONDATA,
	/* application/x-kexiproject-shortcut */
	APPLICATION_X_KEXIPROJECT_SHORTCUT,
	/* application/x-kexiproject-sqlite2 */
	APPLICATION_X_KEXIPROJECT_SQLITE2,
	/* application/x-kexiproject-sqlite3 */
	APPLICATION_X_KEXIPROJECT_SQLITE3,
	/* application/x-kformula */
	APPLICATION_X_KFORMULA,
	/* application/x-killustrator */
	APPLICATION_X_KILLUSTRATOR,
	/* application/x-kivio */
	APPLICATION_X_KIVIO,
	/* application/x-kontour */
	APPLICATION_X_KONTOUR,
	/* application/x-kpovmodeler */
	APPLICATION_X_KPOVMODELER,
	/* application/x-kpresenter */
	APPLICATION_X_KPRESENTER,
	/* application/x-krita */
	APPLICATION_X_KRITA,
	/* application/x-kspread */
	APPLICATION_X_KSPREAD,
	/* application/x-kspread-crypt */
	APPLICATION_X_KSPREAD_CRYPT,
	/* application/x-ksysv-package */
	APPLICATION_X_KSYSV_PACKAGE,
	/* application/x-kugar */
	APPLICATION_X_KUGAR,
	/* application/x-kword */
	APPLICATION_X_KWORD,
	/* application/x-kword-crypt */
	APPLICATION_X_KWORD_CRYPT,
	/* application/x-lanalyzer */
	APPLICATION_X_LANALYZER,
	/* application/x-lha */
	APPLICATION_X_LHA,
	/* application/x-lhz */
	APPLICATION_X_LHZ,
	/* application/x-lrzip */
	APPLICATION_X_LRZIP,
	/* application/x-lrzip-compressed-tar */
	APPLICATION_X_LRZIP_COMPRESSED_TAR,
	/* application/x-lyx */
	APPLICATION_X_LYX,
	/* application/x-lzip */
	APPLICATION_X_LZIP,
	/* application/x-lzma */
	APPLICATION_X_LZMA,
	/* application/x-lzma-compressed-tar */
	APPLICATION_X_LZMA_COMPRESSED_TAR,
	/* application/x-lzop */
	APPLICATION_X_LZOP,
	/* application/x-m4 */
	APPLICATION_X_M4,
	/* application/x-macbinary */
	APPLICATION_X_MACBINARY,
	/* application/x-magicpoint */
	APPLICATION_X_MAGICPOINT,
	/* application/x-markaby */
	APPLICATION_X_MARKABY,
	/* application/x-matroska */
	APPLICATION_X_MATROSKA,
	/* application/x-meld-comparison */
	APPLICATION_X_MELD_COMPARISON,
	/* application/x-mif */
	APPLICATION_X_MIF,
	/* application/x-mimearchive */
	APPLICATION_X_MIMEARCHIVE,
	/* application/x-mobipocket-ebook */
	APPLICATION_X_MOBIPOCKET_EBOOK,
	/* application/x-mozilla-bookmarks */
	APPLICATION_X_MOZILLA_BOOKMARKS,
	/* application/x-mps */
	APPLICATION_X_MPS,
	/* application/x-ms-dos-executable */
	APPLICATION_X_MS_DOS_EXECUTABLE,
	/* application/x-ms-wim */
	APPLICATION_X_MS_WIM,
	/* application/x-msi */
	APPLICATION_X_MSI,
	/* application/x-mswinurl */
	APPLICATION_X_MSWINURL,
	/* application/x-mswrite */
	APPLICATION_X_MSWRITE,
	/* application/x-msx-rom */
	APPLICATION_X_MSX_ROM,
	/* application/x-n64-rom */
	APPLICATION_X_N64_ROM,
	/* application/x-nautilus-link */
	APPLICATION_X_NAUTILUS_LINK,
	/* application/x-navi-animation */
	APPLICATION_X_NAVI_ANIMATION,
	/* application/x-nes-rom */
	APPLICATION_X_NES_ROM,
	/* application/x-netcdf */
	APPLICATION_X_NETCDF,
	/* application/x-netinstobserver */
	APPLICATION_X_NETINSTOBSERVER,
	/* application/x-netshow-channel */
	APPLICATION_X_NETSHOW_CHANNEL,
	/* application/x-nettl */
	APPLICATION_X_NETTL,
	/* application/x-nintendo-ds-rom */
	APPLICATION_X_NINTENDO_DS_ROM,
	/* application/x-nzb */
	APPLICATION_X_NZB,
	/* application/x-object */
	APPLICATION_X_OBJECT,
	/* application/x-ole-storage */
	APPLICATION_X_OLE_STORAGE,
	/* application/x-oleo */
	APPLICATION_X_OLEO,
	/* application/x-pak */
	APPLICATION_X_PAK,
	/* application/x-par2 */
	APPLICATION_X_PAR2,
	/* application/x-pcapng */
	APPLICATION_X_PCAPNG,
	/* application/x-pef-executable */
	APPLICATION_X_PEF_EXECUTABLE,
	/* application/x-pem-file */
	APPLICATION_X_PEM_FILE,
	/* application/x-pem-key */
	APPLICATION_X_PEM_KEY,
	/* application/x-perl */
	APPLICATION_X_PERL,
	/* application/x-php */
	APPLICATION_X_PHP,
	/* application/x-pkcs12 */
	APPLICATION_X_PKCS12,
	/* application/x-pkcs7-certificates */
	APPLICATION_X_PKCS7_CERTIFICATES,
	/* application/x-planperfect */
	APPLICATION_X_PLANPERFECT,
	/* application/x-pocket-word */
	APPLICATION_X_POCKET_WORD,
	/* application/x-profile */
	APPLICATION_X_PROFILE,
	/* application/x-pw */
	APPLICATION_X_PW,
	/* application/x-python-bytecode */
	APPLICATION_X_PYTHON_BYTECODE,
	/* application/x-qtiplot */
	APPLICATION_X_QTIPLOT,
	/* application/x-quattropro */
	APPLICATION_X_QUATTROPRO,
	/* application/x-quicktime-media-link */
	APPLICATION_X_QUICKTIME_MEDIA_LINK,
	/* application/x-qw */
	APPLICATION_X_QW,
	/* application/x-radcom */
	APPLICATION_X_RADCOM,
	/* application/x-rar */
	APPLICATION_X_RAR,
	/* application/x-raw-disk-image */
	APPLICATION_X_RAW_DISK_IMAGE,
	/* application/x-raw-disk-image-xz-compressed */
	APPLICATION_X_RAW_DISK_IMAGE_XZ_COMPRESSED,
	/* application/x-rpm */
	APPLICATION_X_RPM,
	/* application/x-ruby */
	APPLICATION_X_RUBY,
	/* application/x-sami */
	APPLICATION_X_SAMI,
	/* application/x-sc */
	APPLICATION_X_SC,
	/* application/x-shar */
	APPLICATION_X_SHAR,
	/* application/x-shared-library-la */
	APPLICATION_X_SHARED_LIBRARY_LA,
	/* application/x-sharedlib */
	APPLICATION_X_SHAREDLIB,
	/* application/x-shellscript */
	APPLICATION_X_SHELLSCRIPT,
	/* application/x-shorten */
	APPLICATION_X_SHORTEN,
	/* application/x-siag */
	APPLICATION_X_SIAG,
	/* application/x-slp */
	APPLICATION_X_SLP,
	/* application/x-smaf */
	APPLICATION_X_SMAF,
	/* application/x-sms-rom */
	APPLICATION_X_SMS_ROM,
	/* application/x-snoop */
	APPLICATION_X_SNOOP,
	/* application/x-source-rpm */
	APPLICATION_X_SOURCE_RPM,
	/* application/x-spkac */
	APPLICATION_X_SPKAC,
	/* application/x-spkac+base64 */
	APPLICATION_X_SPKAC_BASE64,
	/* application/x-spss-por */
	APPLICATION_X_SPSS_POR,
	/* application/x-spss-sav */
	APPLICATION_X_SPSS_SAV,
	/* application/x-sqlite2 */
	APPLICATION_X_SQLITE2,
	/* application/x-sqlite3 */
	APPLICATION_X_SQLITE3,
	/* application/x-stuffit */
	APPLICATION_X_STUFFIT,
	/* application/x-subrip */
	APPLICATION_X_SUBRIP,
	/* application/x-sv4cpio */
	APPLICATION_X_SV4CPIO,
	/* application/x-sv4crc */
	APPLICATION_X_SV4CRC,
	/* application/x-t602 */
	APPLICATION_X_T602,
	/* application/x-tar */
	APPLICATION_X_TAR,
	/* application/x-tarz */
	APPLICATION_X_TARZ,
	/* application/x-tex-gf */
	APPLICATION_X_TEX_GF,
	/* application/x-tex-pk */
	APPLICATION_X_TEX_PK,
	/* application/x-tgif */
	APPLICATION_X_TGIF,
	/* application/x-theme */
	APPLICATION_X_THEME,
	/* application/x-toutdoux */
	APPLICATION_X_TOUTDOUX,
	/* application/x-trash */
	APPLICATION_X_TRASH,
	/* application/x-trig */
	APPLICATION_X_TRIG,
	/* application/x-troff-man */
	APPLICATION_X_TROFF_MAN,
	/* application/x-troff-man-compressed */
	APPLICATION_X_TROFF_MAN_COMPRESSED,
	/* application/x-tzo */
	APPLICATION_X_TZO,
	/* application/x-ufraw */
	APPLICATION_X_UFRAW,
	/* application/x-ustar */
	APPLICATION_X_USTAR,
	/* application/x-virtualbox-hdd */
	APPLICATION_X_VIRTUALBOX_HDD,
	/* application/x-virtualbox-ova */
	APPLICATION_X_VIRTUALBOX_OVA,
	/* application/x-virtualbox-ovf */
	APPLICATION_X_VIRTUALBOX_OVF,
	/* application/x-virtualbox-vbox */
	APPLICATION_X_VIRTUALBOX_VBOX,
	/* application/x-virtualbox-vbox-extpack */
	APPLICATION_X_VIRTUALBOX_VBOX_EXTPACK,
	/* application/x-virtualbox-vdi */
	APPLICATION_X_VIRTUALBOX_VDI,
	/* application/x-virtualbox-vhd */
	APPLICATION_X_VIRTUALBOX_VHD,
	/* application/x-virtualbox-vmdk */
	APPLICATION_X_VIRTUALBOX_VMDK,
	/* application/x-visualnetworks */
	APPLICATION_X_VISUALNETWORKS,
	/* application/x-wais-source */
	APPLICATION_X_WAIS_SOURCE,
	/* application/x-windows-themepack */
	APPLICATION_X_WINDOWS_THEMEPACK,
	/* application/x-wpg */
	APPLICATION_X_WPG,
	/* application/x-wwf */
	APPLICATION_X_WWF,
	/* application/x-x509-ca-cert */
	APPLICATION_X_X509_CA_CERT,
	/* application/x-xbel */
	APPLICATION_X_XBEL,
	/* application/x-xliff */
	APPLICATION_X_XLIFF,
	/* application/x-xpinstall */
	APPLICATION_X_XPINSTALL,
	/* application/x-xz */
	APPLICATION_X_XZ,
	/* application/x-xz-compressed-tar */
	APPLICATION_X_XZ_COMPRESSED_TAR,
	/* application/x-xzpdf */
	APPLICATION_X_XZPDF,
	/* application/x-yaml */
	APPLICATION_X_YAML,
	/* application/x-zerosize */
	APPLICATION_X_ZEROSIZE,
	/* application/x-zoo */
	APPLICATION_X_ZOO,
	/* application/xhtml+xml */
	APPLICATION_XHTML_XML,
	/* application/xml */
	APPLICATION_XML,
	/* application/xml-dtd */
	APPLICATION_XML_DTD,
	/* application/xml-external-parsed-entity */
	APPLICATION_XML_EXTERNAL_PARSED_ENTITY,
	/* application/xslt+xml */
	APPLICATION_XSLT_XML,
	/* application/xspf+xml */
	APPLICATION_XSPF_XML,
	/* application/zip */
	APPLICATION_ZIP,
	/* audio/AMR */
	AUDIO_AMR,
	/* audio/AMR-WB */
	AUDIO_AMR_WB,
	/* audio/aac */
	AUDIO_AAC,
	/* audio/ac3 */
	AUDIO_AC3,
	/* audio/annodex */
	AUDIO_ANNODEX,
	/* audio/basic */
	AUDIO_BASIC,
	/* audio/flac */
	AUDIO_FLAC,
	/* audio/midi */
	AUDIO_MIDI,
	/* audio/mp2 */
	AUDIO_MP2,
	/* audio/mp4 */
	AUDIO_MP4,
	/* audio/mpeg */
	AUDIO_MPEG,
	/* audio/ogg */
	AUDIO_OGG,
	/* audio/prs.sid */
	AUDIO_PRS_SID,
	/* audio/vnd.dts */
	AUDIO_VND_DTS,
	/* audio/vnd.dts.hd */
	AUDIO_VND_DTS_HD,
	/* audio/vnd.rn-realaudio */
	AUDIO_VND_RN_REALAUDIO,
	/* audio/webm */
	AUDIO_WEBM,
	/* audio/x-adpcm */
	AUDIO_X_ADPCM,
	/* audio/x-aifc */
	AUDIO_X_AIFC,
	/* audio/x-aiff */
	AUDIO_X_AIFF,
	/* audio/x-amzxml */
	AUDIO_X_AMZXML,
	/* audio/x-ape */
	AUDIO_X_APE,
	/* audio/x-flac+ogg */
	AUDIO_X_FLAC_OGG,
	/* audio/x-gsm */
	AUDIO_X_GSM,
	/* audio/x-iriver-pla */
	AUDIO_X_IRIVER_PLA,
	/* audio/x-it */
	AUDIO_X_IT,
	/* audio/x-m4b */
	AUDIO_X_M4B,
	/* audio/x-matroska */
	AUDIO_X_MATROSKA,
	/* audio/x-minipsf */
	AUDIO_X_MINIPSF,
	/* audio/x-mo3 */
	AUDIO_X_MO3,
	/* audio/x-mod */
	AUDIO_X_MOD,
	/* audio/x-mpegurl */
	AUDIO_X_MPEGURL,
	/* audio/x-ms-asx */
	AUDIO_X_MS_ASX,
	/* audio/x-ms-wma */
	AUDIO_X_MS_WMA,
	/* audio/x-musepack */
	AUDIO_X_MUSEPACK,
	/* audio/x-opus+ogg */
	AUDIO_X_OPUS_OGG,
	/* audio/x-psf */
	AUDIO_X_PSF,
	/* audio/x-psflib */
	AUDIO_X_PSFLIB,
	/* audio/x-riff */
	AUDIO_X_RIFF,
	/* audio/x-s3m */
	AUDIO_X_S3M,
	/* audio/x-scpls */
	AUDIO_X_SCPLS,
	/* audio/x-speex */
	AUDIO_X_SPEEX,
	/* audio/x-speex+ogg */
	AUDIO_X_SPEEX_OGG,
	/* audio/x-stm */
	AUDIO_X_STM,
	/* audio/x-tta */
	AUDIO_X_TTA,
	/* audio/x-voc */
	AUDIO_X_VOC,
	/* audio/x-vorbis+ogg */
	AUDIO_X_VORBIS_OGG,
	/* audio/x-wav */
	AUDIO_X_WAV,
	/* audio/x-wavpack */
	AUDIO_X_WAVPACK,
	/* audio/x-wavpack-correction */
	AUDIO_X_WAVPACK_CORRECTION,
	/* audio/x-xi */
	AUDIO_X_XI,
	/* audio/x-xm */
	AUDIO_X_XM,
	/* audio/x-xmf */
	AUDIO_X_XMF,
	/* image/bmp */
	IMAGE_BMP,
	/* image/cgm */
	IMAGE_CGM,
	/* image/dpx */
	IMAGE_DPX,
	/* image/fax-g3 */
	IMAGE_FAX_G3,
	/* image/fits */
	IMAGE_FITS,
	/* image/g3fax */
	IMAGE_G3FAX,
	/* image/gif */
	IMAGE_GIF,
	/* image/ief */
	IMAGE_IEF,
	/* image/jp2 */
	IMAGE_JP2,
	/* image/jpeg */
	IMAGE_JPEG,
	/* image/openraster */
	IMAGE_OPENRASTER,
	/* image/png */
	IMAGE_PNG,
	/* image/rle */
	IMAGE_RLE,
	/* image/svg+xml */
	IMAGE_SVG_XML,
	/* image/svg+xml-compressed */
	IMAGE_SVG_XML_COMPRESSED,
	/* image/tiff */
	IMAGE_TIFF,
	/* image/vnd.adobe.photoshop */
	IMAGE_VND_ADOBE_PHOTOSHOP,
	/* image/vnd.djvu */
	IMAGE_VND_DJVU,
	/* image/vnd.dwg */
	IMAGE_VND_DWG,
	/* image/vnd.dxf */
	IMAGE_VND_DXF,
	/* image/vnd.microsoft.icon */
	IMAGE_VND_MICROSOFT_ICON,
	/* image/vnd.ms-modi */
	IMAGE_VND_MS_MODI,
	/* image/vnd.rn-realpix */
	IMAGE_VND_RN_REALPIX,
	/* image/vnd.wap.wbmp */
	IMAGE_VND_WAP_WBMP,
	/* image/x-3ds */
	IMAGE_X_3DS,
	/* image/x-adobe-dng */
	IMAGE_X_ADOBE_DNG,
	/* image/x-apple-ios-png */
	IMAGE_X_APPLE_IOS_PNG,
	/* image/x-applix-graphics */
	IMAGE_X_APPLIX_GRAPHICS,
	/* image/x-bzeps */
	IMAGE_X_BZEPS,
	/* image/x-canon-cr2 */
	IMAGE_X_CANON_CR2,
	/* image/x-canon-crw */
	IMAGE_X_CANON_CRW,
	/* image/x-cmu-raster */
	IMAGE_X_CMU_RASTER,
	/* image/x-compressed-xcf */
	IMAGE_X_COMPRESSED_XCF,
	/* image/x-dcraw */
	IMAGE_X_DCRAW,
	/* image/x-dds */
	IMAGE_X_DDS,
	/* image/x-dib */
	IMAGE_X_DIB,
	/* image/x-emf */
	IMAGE_X_EMF,
	/* image/x-eps */
	IMAGE_X_EPS,
	/* image/x-exr */
	IMAGE_X_EXR,
	/* image/x-fpx */
	IMAGE_X_FPX,
	/* image/x-fuji-raf */
	IMAGE_X_FUJI_RAF,
	/* image/x-gzeps */
	IMAGE_X_GZEPS,
	/* image/x-icns */
	IMAGE_X_ICNS,
	/* image/x-ilbm */
	IMAGE_X_ILBM,
	/* image/x-jng */
	IMAGE_X_JNG,
	/* image/x-kodak-dcr */
	IMAGE_X_KODAK_DCR,
	/* image/x-kodak-k25 */
	IMAGE_X_KODAK_K25,
	/* image/x-kodak-kdc */
	IMAGE_X_KODAK_KDC,
	/* image/x-lwo */
	IMAGE_X_LWO,
	/* image/x-lws */
	IMAGE_X_LWS,
	/* image/x-macpaint */
	IMAGE_X_MACPAINT,
	/* image/x-minolta-mrw */
	IMAGE_X_MINOLTA_MRW,
	/* image/x-msod */
	IMAGE_X_MSOD,
	/* image/x-niff */
	IMAGE_X_NIFF,
	/* image/x-nikon-nef */
	IMAGE_X_NIKON_NEF,
	/* image/x-olympus-orf */
	IMAGE_X_OLYMPUS_ORF,
	/* image/x-panasonic-raw */
	IMAGE_X_PANASONIC_RAW,
	/* image/x-panasonic-raw2 */
	IMAGE_X_PANASONIC_RAW2,
	/* image/x-pcx */
	IMAGE_X_PCX,
	/* image/x-pentax-pef */
	IMAGE_X_PENTAX_PEF,
	/* image/x-photo-cd */
	IMAGE_X_PHOTO_CD,
	/* image/x-pict */
	IMAGE_X_PICT,
	/* image/x-portable-anymap */
	IMAGE_X_PORTABLE_ANYMAP,
	/* image/x-portable-bitmap */
	IMAGE_X_PORTABLE_BITMAP,
	/* image/x-portable-graymap */
	IMAGE_X_PORTABLE_GRAYMAP,
	/* image/x-portable-pixmap */
	IMAGE_X_PORTABLE_PIXMAP,
	/* image/x-quicktime */
	IMAGE_X_QUICKTIME,
	/* image/x-rgb */
	IMAGE_X_RGB,
	/* image/x-sgi */
	IMAGE_X_SGI,
	/* image/x-sigma-x3f */
	IMAGE_X_SIGMA_X3F,
	/* image/x-skencil */
	IMAGE_X_SKENCIL,
	/* image/x-sony-arw */
	IMAGE_X_SONY_ARW,
	/* image/x-sony-sr2 */
	IMAGE_X_SONY_SR2,
	/* image/x-sony-srf */
	IMAGE_X_SONY_SRF,
	/* image/x-sun-raster */
	IMAGE_X_SUN_RASTER,
	/* image/x-tga */
	IMAGE_X_TGA,
	/* image/x-win-bitmap */
	IMAGE_X_WIN_BITMAP,
	/* image/x-wmf */
	IMAGE_X_WMF,
	/* image/x-xbitmap */
	IMAGE_X_XBITMAP,
	/* image/x-xcf */
	IMAGE_X_XCF,
	/* image/x-xcursor */
	IMAGE_X_XCURSOR,
	/* image/x-xfig */
	IMAGE_X_XFIG,
	/* image/x-xpixmap */
	IMAGE_X_XPIXMAP,
	/* image/x-xwindowdump */
	IMAGE_X_XWINDOWDUMP,
	/* inode/blockdevice */
	INODE_BLOCKDEVICE,
	/* inode/chardevice */
	INODE_CHARDEVICE,
	/* inode/directory */
	INODE_DIRECTORY,
	/* inode/fifo */
	INODE_FIFO,
	/* inode/mount-point */
	INODE_MOUNT_POINT,
	/* inode/socket */
	INODE_SOCKET,
	/* inode/symlink */
	INODE_SYMLINK,
	/* message/delivery-status */
	MESSAGE_DELIVERY_STATUS,
	/* message/disposition-notification */
	MESSAGE_DISPOSITION_NOTIFICATION,
	/* message/external-body */
	MESSAGE_EXTERNAL_BODY,
	/* message/news */
	MESSAGE_NEWS,
	/* message/partial */
	MESSAGE_PARTIAL,
	/* message/rfc822 */
	MESSAGE_RFC822,
	/* message/x-gnu-rmail */
	MESSAGE_X_GNU_RMAIL,
	/* model/vrml */
	MODEL_VRML,
	/* multipart/alternative */
	MULTIPART_ALTERNATIVE,
	/* multipart/appledouble */
	MULTIPART_APPLEDOUBLE,
	/* multipart/digest */
	MULTIPART_DIGEST,
	/* multipart/encrypted */
	MULTIPART_ENCRYPTED,
	/* multipart/mixed */
	MULTIPART_MIXED,
	/* multipart/related */
	MULTIPART_RELATED,
	/* multipart/report */
	MULTIPART_REPORT,
	/* multipart/signed */
	MULTIPART_SIGNED,
	/* multipart/x-mixed-replace */
	MULTIPART_X_MIXED_REPLACE,
	/* text/cache-manifest */
	TEXT_CACHE_MANIFEST,
	/* text/calendar */
	TEXT_CALENDAR,
	/* text/css */
	TEXT_CSS,
	/* text/csv */
	TEXT_CSV,
	/* text/enriched */
	TEXT_ENRICHED,
	/* text/html */
	TEXT_HTML,
	/* text/htmlh */
	TEXT_HTMLH,
	/* text/plain */
	TEXT_PLAIN,
	/* text/rfc822-headers */
	TEXT_RFC822_HEADERS,
	/* text/richtext */
	TEXT_RICHTEXT,
	/* text/sgml */
	TEXT_SGML,
	/* text/spreadsheet */
	TEXT_SPREADSHEET,
	/* text/tab-separated-values */
	TEXT_TAB_SEPARATED_VALUES,
	/* text/troff */
	TEXT_TROFF,
	/* text/vcard */
	TEXT_VCARD,
	/* text/vnd.graphviz */
	TEXT_VND_GRAPHVIZ,
	/* text/vnd.rn-realtext */
	TEXT_VND_RN_REALTEXT,
	/* text/vnd.sun.j2me.app-descriptor */
	TEXT_VND_SUN_J2ME_APP_DESCRIPTOR,
	/* text/vnd.trolltech.linguist */
	TEXT_VND_TROLLTECH_LINGUIST,
	/* text/vnd.wap.wml */
	TEXT_VND_WAP_WML,
	/* text/vnd.wap.wmlscript */
	TEXT_VND_WAP_WMLSCRIPT,
	/* text/vtt */
	TEXT_VTT,
	/* text/x-adasrc */
	TEXT_X_ADASRC,
	/* text/x-apport */
	TEXT_X_APPORT,
	/* text/x-apt-sources-list */
	TEXT_X_APT_SOURCES_LIST,
	/* text/x-authors */
	TEXT_X_AUTHORS,
	/* text/x-bibtex */
	TEXT_X_BIBTEX,
	/* text/x-c++hdr */
	TEXT_X_CPPHDR,
	/* text/x-c++src */
	TEXT_X_CPPSRC,
	/* text/x-changelog */
	TEXT_X_CHANGELOG,
	/* text/x-chdr */
	TEXT_X_CHDR,
	/* text/x-cmake */
	TEXT_X_CMAKE,
	/* text/x-cobol */
	TEXT_X_COBOL,
	/* text/x-copying */
	TEXT_X_COPYING,
	/* text/x-credits */
	TEXT_X_CREDITS,
	/* text/x-csharp */
	TEXT_X_CSHARP,
	/* text/x-csrc */
	TEXT_X_CSRC,
	/* text/x-dcl */
	TEXT_X_DCL,
	/* text/x-dsl */
	TEXT_X_DSL,
	/* text/x-dsrc */
	TEXT_X_DSRC,
	/* text/x-eiffel */
	TEXT_X_EIFFEL,
	/* text/x-emacs-lisp */
	TEXT_X_EMACS_LISP,
	/* text/x-erlang */
	TEXT_X_ERLANG,
	/* text/x-fortran */
	TEXT_X_FORTRAN,
	/* text/x-gettext-translation */
	TEXT_X_GETTEXT_TRANSLATION,
	/* text/x-gettext-translation-template */
	TEXT_X_GETTEXT_TRANSLATION_TEMPLATE,
	/* text/x-go */
	TEXT_X_GO,
	/* text/x-google-video-pointer */
	TEXT_X_GOOGLE_VIDEO_POINTER,
	/* text/x-haskell */
	TEXT_X_HASKELL,
	/* text/x-iMelody */
	TEXT_X_IMELODY,
	/* text/x-idl */
	TEXT_X_IDL,
	/* text/x-install */
	TEXT_X_INSTALL,
	/* text/x-iptables */
	TEXT_X_IPTABLES,
	/* text/x-java */
	TEXT_X_JAVA,
	/* text/x-ldif */
	TEXT_X_LDIF,
	/* text/x-lilypond */
	TEXT_X_LILYPOND,
	/* text/x-literate-haskell */
	TEXT_X_LITERATE_HASKELL,
	/* text/x-log */
	TEXT_X_LOG,
	/* text/x-lua */
	TEXT_X_LUA,
	/* text/x-makefile */
	TEXT_X_MAKEFILE,
	/* text/x-markdown */
	TEXT_X_MARKDOWN,
	/* text/x-matlab */
	TEXT_X_MATLAB,
	/* text/x-microdvd */
	TEXT_X_MICRODVD,
	/* text/x-moc */
	TEXT_X_MOC,
	/* text/x-modelica */
	TEXT_X_MODELICA,
	/* text/x-mof */
	TEXT_X_MOF,
	/* text/x-mpsub */
	TEXT_X_MPSUB,
	/* text/x-mrml */
	TEXT_X_MRML,
	/* text/x-ms-regedit */
	TEXT_X_MS_REGEDIT,
	/* text/x-mup */
	TEXT_X_MUP,
	/* text/x-nfo */
	TEXT_X_NFO,
	/* text/x-objcsrc */
	TEXT_X_OBJCSRC,
	/* text/x-ocaml */
	TEXT_X_OCAML,
	/* text/x-ocl */
	TEXT_X_OCL,
	/* text/x-ooc */
	TEXT_X_OOC,
	/* text/x-opml+xml */
	TEXT_X_OPML_XML,
	/* text/x-pascal */
	TEXT_X_PASCAL,
	/* text/x-patch */
	TEXT_X_PATCH,
	/* text/x-python */
	TEXT_X_PYTHON,
	/* text/x-qml */
	TEXT_X_QML,
	/* text/x-readme */
	TEXT_X_README,
	/* text/x-reject */
	TEXT_X_REJECT,
	/* text/x-rpm-spec */
	TEXT_X_RPM_SPEC,
	/* text/x-scala */
	TEXT_X_SCALA,
	/* text/x-scheme */
	TEXT_X_SCHEME,
	/* text/x-setext */
	TEXT_X_SETEXT,
	/* text/x-ssa */
	TEXT_X_SSA,
	/* text/x-subviewer */
	TEXT_X_SUBVIEWER,
	/* text/x-svhdr */
	TEXT_X_SVHDR,
	/* text/x-svsrc */
	TEXT_X_SVSRC,
	/* text/x-tcl */
	TEXT_X_TCL,
	/* text/x-tex */
	TEXT_X_TEX,
	/* text/x-texinfo */
	TEXT_X_TEXINFO,
	/* text/x-troff-me */
	TEXT_X_TROFF_ME,
	/* text/x-troff-mm */
	TEXT_X_TROFF_MM,
	/* text/x-troff-ms */
	TEXT_X_TROFF_MS,
	/* text/x-txt2tags */
	TEXT_X_TXT2TAGS,
	/* text/x-uil */
	TEXT_X_UIL,
	/* text/x-uri */
	TEXT_X_URI,
	/* text/x-uuencode */
	TEXT_X_UUENCODE,
	/* text/x-vala */
	TEXT_X_VALA,
	/* text/x-verilog */
	TEXT_X_VERILOG,
	/* text/x-vhdl */
	TEXT_X_VHDL,
	/* text/x-xmi */
	TEXT_X_XMI,
	/* text/x-xslfo */
	TEXT_X_XSLFO,
	/* text/xmcd */
	TEXT_XMCD,
	/* video/3gpp */
	VIDEO_3GPP,
	/* video/3gpp2 */
	VIDEO_3GPP2,
	/* video/annodex */
	VIDEO_ANNODEX,
	/* video/dv */
	VIDEO_DV,
	/* video/isivideo */
	VIDEO_ISIVIDEO,
	/* video/mp2t */
	VIDEO_MP2T,
	/* video/mp4 */
	VIDEO_MP4,
	/* video/mpeg */
	VIDEO_MPEG,
	/* video/ogg */
	VIDEO_OGG,
	/* video/quicktime */
	VIDEO_QUICKTIME,
	/* video/vivo */
	VIDEO_VIVO,
	/* video/vnd.mpegurl */
	VIDEO_VND_MPEGURL,
	/* video/vnd.rn-realvideo */
	VIDEO_VND_RN_REALVIDEO,
	/* video/wavelet */
	VIDEO_WAVELET,
	/* video/webm */
	VIDEO_WEBM,
	/* video/x-anim */
	VIDEO_X_ANIM,
	/* video/x-flic */
	VIDEO_X_FLIC,
	/* video/x-flv */
	VIDEO_X_FLV,
	/* video/x-javafx */
	VIDEO_X_JAVAFX,
	/* video/x-matroska */
	VIDEO_X_MATROSKA,
	/* video/x-matroska-3d */
	VIDEO_X_MATROSKA_3D,
	/* video/x-mng */
	VIDEO_X_MNG,
	/* video/x-ms-asf */
	VIDEO_X_MS_ASF,
	/* video/x-ms-wmv */
	VIDEO_X_MS_WMV,
	/* video/x-msvideo */
	VIDEO_X_MSVIDEO,
	/* video/x-nsv */
	VIDEO_X_NSV,
	/* video/x-ogm+ogg */
	VIDEO_X_OGM_OGG,
	/* video/x-sgi-movie */
	VIDEO_X_SGI_MOVIE,
	/* video/x-theora+ogg */
	VIDEO_X_THEORA_OGG,
	/* x-content/audio-cdda */
	X_CONTENT_AUDIO_CDDA,
	/* x-content/audio-dvd */
	X_CONTENT_AUDIO_DVD,
	/* x-content/audio-player */
	X_CONTENT_AUDIO_PLAYER,
	/* x-content/blank-bd */
	X_CONTENT_BLANK_BD,
	/* x-content/blank-cd */
	X_CONTENT_BLANK_CD,
	/* x-content/blank-dvd */
	X_CONTENT_BLANK_DVD,
	/* x-content/blank-hddvd */
	X_CONTENT_BLANK_HDDVD,
	/* x-content/ebook-reader */
	X_CONTENT_EBOOK_READER,
	/* x-content/image-dcf */
	X_CONTENT_IMAGE_DCF,
	/* x-content/image-picturecd */
	X_CONTENT_IMAGE_PICTURECD,
	/* x-content/software */
	X_CONTENT_SOFTWARE,
	/* x-content/unix-software */
	X_CONTENT_UNIX_SOFTWARE,
	/* x-content/video-bluray */
	X_CONTENT_VIDEO_BLURAY,
	/* x-content/video-dvd */
	X_CONTENT_VIDEO_DVD,
	/* x-content/video-hddvd */
	X_CONTENT_VIDEO_HDDVD,
	/* x-content/video-svcd */
	X_CONTENT_VIDEO_SVCD,
	/* x-content/video-vcd */
	X_CONTENT_VIDEO_VCD,
	/* x-content/win32-software */
	X_CONTENT_WIN32_SOFTWARE,
	/* x-epoc/x-sisx-app */
	X_EPOC_X_SISX_APP
};


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MIME_TYPES_H__ */

