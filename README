qPBReader epub reader


Developed on and for pocketbook 623 with firmware 4. Also tested as working on 626 with firmware 4.
Runs on basic, but is almost unusable since it is touch-based.
Only supports epub files.

Credits:
Qt
Calibre, entiere chunks of code have been ported to C++, ebook-viewer javascript stuff is used without any modification.
Various people on stackoverflow etc. for small portions of code, too many to remember and give proper credits.
libxml, libxslt
inkview sdk from pocketbook.


Features:
- written in Qt 4
- reads epub files, heavily based on calibre ebook-viewer (javascript paged display technology)
- document navigation through table of contents
- supports mathjax (though painfully slow computation!)
- integrated with pocketbook system:
    - bookstate (page number)
    - restarts automatically if pb shuts down while a book is opened,
    - pb dictionary lookups through tap&hold gesture on word, and simple tap when dictionary dialog is opened
    - book icon in task manager
- removable/movable "back" button after following a link (did not get transparency working :( )
- allows font changing (if epub stylesheet allows it)
- allows margin adjustments
- screen rotation support
- respects epub stylesheet, displays epub as it is intended to be, *in the limits of webkit capabilities*
- true page count, but count is manually triggered (automatically reset after any layout manipulation)
- optional hyphenation (slightly slows down section loading)
- immediate setting changes visible without going into and back from full screen menus
- customizable font configuration (see "About fonts" for more details)

Missing:
- bookmarks
- no notes
- no interface with note taking software

I do not plan implenting those features. I understand this could be very desirable, but 1/ my time is limited, 2/ I do not need them.
Bookmarks are almost obvious to add, thanks to "cfi" object.


About fonts:
Qt pocketbook integration plugin uses fontconfig as font backend. There are two issues with default configuration:
- font cache is declared in /tmp. So cache is lost after each reboot. This is why the integrated browser is quicker to at second and more launch.
- font directory is "/ebrmain/fonts" which limits font choice to what is available in the firmware.
To solve that:
- qPBReader uses an alternate font config file (through FONTCONFIG_FILE environment variable).
- "/mnt/ext1/system/fonts" and "/mnt/ext1/system/fonts/adobe/fonts" directories have been added to provided fontconfig file.
- It is impossible for PB fontconfig to work on FAT (strace shows that symbolic link creation fails). So "/mnt/secure/fccache" is used. Thus, first startup will be slow, depending on how many fonts are present in the declared directories.
You can modify FONTCONFIG_FILE value inside /mnt/ext1/system/bin/qPBReader.app script file, or edit provided  /mnt/ext1/system/config/qPBReaderFonts.conf if you need something specific.
You can also ask qPBReader to ignore embedded fonts. This is done through javascript, all @font-face and font-family declarations will be discarded.

About settings:
qPBReader uses default settings value ("global"), located inside /mnt/ext1/system/config/qPBReader.conf. This file cannot be edited through the graphical user interface when reading books.
However, a specific program allows to modify it (in fact qPBReader is launched in a special mode through the command line "qPBReader -config").
When opening a book, if a setting has not already been read from book-specific setting file (located in /mnt/ext1/share/qPBReader/data/<book MD5 fingerprint>.cfg), default value is used and written in book-specific setting.
Data files contain some book-related information, like sections, zoom value, fonts, page count, etc.


About page counting:
Page counting is not performed by default. Is it heavily dependant on settings.
When you have defined font-size etc., you have to manually launch computation.
While running, qPBReader is still able to turn pages, etc. but will be less responsive than usual till computation ends.
Page counting works as follow:
- a section is invisibly loaded, with same settings as currently applied ones.
- number of pages are counted for this section with calibre js stuff
- goto first step for next section, if any.
So the result is the exact number of pages.
Page count results are immediatly invalidated when modifying settings.
Page count (current/total) is used for persistence in bookstate pocketbook database.
I am not completely satisfied with the way it is working since it is the result of a compromise, "exactness" vs "comfort" vs "minimal development effort". However I personally rarely change settings once configured so this is enough for me.


Remarks, known bugs & limitations:
webkit in Qt4 is buggy for @font-face stuff. I implemented a different method than the one present in calibre to handle embedded fonts.
In calibre, the stylesheets are analysed for @font-face, fonts are imported into Qt runtime font database, and stylesheets are fixed if internal font-family mismatch true font file font-family.
The same is done in qPBReader, but at javascript level. This works pretty well on PC, results are not that good on pocketbook.

Qt webkit on pocketbook has some limits with javascript, not all paged display script stuff is working, mathjax won't work.
I do not know what is wrong with the one in firmware 4, but my Qt QPA build works better on this aspect. Some other stuff might be broken, but I have not encountered any problem with qPBReader.
You should copy libQtWebKit.so.4 into /mnt/ext1/system/share/qPBReader/lib (this directory is added to LD_LIBRARY_PATH when launching qPBReader).

qPBReader will not choke on epub files with multiple opf. Only the first one is used (as do most software I have seen), but qPBReader is totally aware of current opf in use. It should not be complicated to add the possibility to switch opf.

qPBReader is as relaxed with epub norm as possible, all epub files I have opened so far were rendered fine.
qPBReader should be able to open and display all reasonably built epub files, but no guarantee can be offered if the epub is broken (missing sections, undeclared files etc.).
qPBReader works reasonably fast, but might be a bit slow when working with big epub sections.
