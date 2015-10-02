<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                exclude-result-prefixes="ncx xsl"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:ncx="http://www.daisy.org/z3986/2005/ncx/"
                xmlns="http://www.w3.org/1999/xhtml">
   <xsl:output method="xml"
               indent="yes"
               encoding="utf-8"
               doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
               doctype-public="-//W3C//DTD XHTML 1.1//EN"/>
   <xsl:preserve-space elements="*"/>
   <xsl:template match="ncx:ncx">
      <html>
         <head>
            <title>Table of contents</title>
            <style type="text/css">
               body
               {
                  font-size: 1em;
                  line-height: 1.3;
                  margin: 0 0 0 0;
               }
               h1
               {
                  font-size: 1.2em;
                  text-align: center;
                  margin-top: 1em;
                  margin-bottom: 1.5em;
               }
               ul
               {
                  list-style-type: none;
               }
               a
               {
                  text-decoration: none;
               }
            </style>
         </head>
         <body>
            <h1><xsl:value-of select="ncx:docTitle/ncx:text"/></h1>
               <ul>
                  <xsl:apply-templates select="ncx:navMap|ncx:pageList"/>
               </ul>
         </body>
      </html>
   </xsl:template>
   <xsl:template match="ncx:navPoint">
      <!-- Only some navPoints have more navPoints -->
      <li>
         <a href="{ncx:content/@src}"><xsl:value-of select="ncx:navLabel/ncx:text"/></a>
         <xsl:if test="ncx:navPoint">
            <ul>
               <xsl:apply-templates select="ncx:navPoint"/>
            </ul>
         </xsl:if>
      </li>
   </xsl:template>
</xsl:stylesheet>
