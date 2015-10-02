<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns="http://www.w3.org/1999/xhtml">
   <xsl:output method="xml"
               indent="yes"
               encoding="utf-8"
               omit-xml-declaration="yes"
               />
    <xsl:param name="pSeparators">&#xA;&#x9;&#x20;&#160;,.;:?!()'&#8217;"</xsl:param>
    <xsl:template match="node()|@*">
        <xsl:copy>
            <xsl:apply-templates select="node()|@*"/>
        </xsl:copy>
    </xsl:template>
    <xsl:template match="text()" name="tokenize">
        <xsl:param name="pString" select="."/>
        <xsl:param name="pMask"
                   select="translate(.,translate(.,$pSeparators,''),'')"/>
        <xsl:param name="pCount" select="1"/>
        <xsl:choose>
            <xsl:when test="not($pString)"/>
            <xsl:when test="$pMask">
                <xsl:variable name="vSeparator"
                              select="substring($pMask,1,1)"/>
                <xsl:variable name="vString"
                              select="substring-before($pString,$vSeparator)"/>
                <xsl:call-template name="tokenize">
                    <xsl:with-param name="pString" select="$vString"/>
                    <xsl:with-param name="pMask"/>
                    <xsl:with-param name="pCount" select="$pCount"/>
                </xsl:call-template>
                <xsl:value-of select="$vSeparator"/>
                <xsl:call-template name="tokenize">
                    <xsl:with-param name="pString"
                               select="substring-after($pString,$vSeparator)"/>
                    <xsl:with-param name="pMask"
                                    select="substring($pMask,2)"/>
                    <xsl:with-param name="pCount"
                                    select="$pCount + boolean($vString)"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <qpbdummy>
                    <xsl:value-of select="$pString"/>
                </qpbdummy>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>
