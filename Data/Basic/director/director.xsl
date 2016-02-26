<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >

  <!-- director definition -->

  <xsl:template match="director">
    <html>

      <head>
        <title>
          Mission Director File:
          <xsl:value-of select="@name"/>
        </title>
        <link rel="stylesheet" href="director.css" type="text/css" />
      </head>

      <body>

        <!-- heading -->

        <table width="99%" class="cue">

          <!-- title -->

          <tr>
            <th colspan="3" class="cue">
              <span class="title" >
                Mission Director File:
                <xsl:value-of select="@name"/>
                <xsl:if test="@comment">
                  <span class="comment">
                    -
                    <xsl:value-of select="@comment" />
                  </span>
                </xsl:if>
              </span>
            </th>
          </tr>

          <!-- documentation -->

          <xsl:if test="documentation">
            <tr>
              <xsl:for-each select="documentation/*">
                <td width="33%">
                  <span class="elementtitle">
                    <xsl:value-of select="concat(translate(substring(name(),1,1),'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ'),substring(name(),2,string-length(name())-1))" />
                  </span>
                  <table width="100%" height="100%" class="element">
                    <xsl:for-each select="@*">
                      <tr>
                        <td width="10%" align="right" valign="top">
                          <xsl:value-of select="concat(translate(substring(name(),1,1),'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ'),substring(name(),2,string-length(name())-1))" />
                        </td>
                        <td class="element">
                          <span class="node">
                            <xsl:value-of select="." />
                          </span>
                        </td>
                      </tr>
                    </xsl:for-each>
                  </table>
                </td>
              </xsl:for-each>
            </tr>
          </xsl:if>
          
        </table>

        <!-- cues -->

        <xsl:apply-templates select="cues" />

        <!-- signature -->

        <xsl:choose>
          <xsl:when test="signature">
            <table width="99%" class="cue">
              <tr>
                <th colspan="3" class="cue">
                  <span class="title" >
                    Signed
                  </span>
                </th>
              </tr>
            </table>
          </xsl:when>
        </xsl:choose>

      </body>

    </html>
  </xsl:template>

  <!-- cue definition -->
  
  <xsl:template match="cue">
    <table width="99%" class="cue">

      <!-- cue header -->

      <tr>
        <th width="10%" class="cue">
          <xsl:value-of select="position()"/>
          of
          <xsl:value-of select="count(../cue)"/>
        </th>
        <th width="75%" class="cue">
          <xsl:choose>
            <xsl:when test="@ref">
              Reference:
              <xsl:value-of select="@ref"/>
            </xsl:when>
            <xsl:when test="@library">
              Library:
              <xsl:value-of select="@name"/>
            </xsl:when>
            <xsl:otherwise>
              Cue:
              <xsl:value-of select="@name"/>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:if test="@comment">
            -
            <span class="comment">
              <xsl:value-of select="@comment"/>
            </span>
          </xsl:if>
        </th>
        <th width="10%" class="cue">
          Version
        </th>
        <th width="5%" class="cue">
          <xsl:value-of select="@version"/>
        </th>
      </tr>

      <!-- documentation -->

      <xsl:if test="documentation">
        <tr>
          <td colspan="4">
            <span class="elementtitle">
              Documentation
            </span>
            <table width="100%" class="element">
              <tr>
                <xsl:for-each select="documentation/*">
                  <td width="16%" valign="top">
                    <span class="elementtitle">
                      <xsl:value-of select="concat(translate(substring(name(),1,1),'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ'),substring(name(),2,string-length(name())-1))" />
                    </span>
                    <xsl:if test="@*">
                      <table width="100%" class="element">
                        <xsl:for-each select="@*">
                          <tr>
                            <td width="10%" align="right" valign="top">
                              <xsl:value-of select="concat(translate(substring(name(),1,1),'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ'),substring(name(),2,string-length(name())-1))" />
                            </td>
                            <td class="element">
                              <span class="node">
                                <xsl:value-of select="." />
                              </span>
                            </td>
                          </tr>
                        </xsl:for-each>
                      </table>
                    </xsl:if>
                    <xsl:for-each select="./*">
                      <table width="100%" class="element">
                        <xsl:for-each select="@*">
                          <tr>
                            <td width="10%" align="right" valign="top">
                              <xsl:value-of select="concat(translate(substring(name(),1,1),'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ'),substring(name(),2,string-length(name())-1))" />
                            </td>
                            <td class="element">
                              <span class="node">
                                <xsl:value-of select="." />
                              </span>
                            </td>
                          </tr>
                        </xsl:for-each>
                      </table>
                    </xsl:for-each>
                  </td>
                </xsl:for-each>
              </tr>
            </table>
          </td>
        </tr>
      </xsl:if>

      <!-- condition -->

      <xsl:if test="condition">
        <tr>
          <td colspan="4">
            <span class="elementtitle">
              Condition
            </span>
            <br />
            <table width="100%" class="element">
              <xsl:apply-templates select="condition" />
            </table>
          </td>
        </tr>
      </xsl:if>

      <!-- timing -->

      <xsl:if test="timing">
        <tr>
          <td colspan="4">
            <span class="elementtitle">
              Timing
            </span>
            <br />
            <table width="100%" class="element">
              <xsl:apply-templates select="timing" />
            </table>
          </td>
         </tr>
      </xsl:if>

      <!-- action -->

      <xsl:if test="action">
        <tr>
          <td colspan="4">
            <span class="elementtitle">
              Action
            </span>
              <xsl:choose>
                <xsl:when test="action/@ref">
                  <span class="elementtitle">
                    - Reference:
                    <xsl:value-of select="action/@ref" />
                  </span>
                </xsl:when>
                <xsl:otherwise>
                  <br />
                  <table width="100%" class="element">
                    <xsl:apply-templates select="action" />
                  </table>
                </xsl:otherwise>
              </xsl:choose>
            </td>
        </tr>
      </xsl:if>

      <!-- force action -->

      <xsl:if test="force">
        <tr>
          <td colspan="4">
            <span class="elementtitle">
              Action
            </span>
            <br />
            <table width="100%" class="element">
              <xsl:apply-templates select="force" />
            </table>
          </td>
        </tr>
      </xsl:if>
      
      <!-- subcues -->
      
      <xsl:if test="cues">
        <tr>
          <td colspan="4">
            <span class="elementtitle">
              Sub-cues
            </span>
            <br />
            <xsl:apply-templates select="cues" />
          </td>
        </tr>
      </xsl:if>
      
    </table>
  </xsl:template>

  <!-- element definition -->

  <xsl:template match="condition//*|timing//*|action//*|force//*">
    <tr>
      <td class="element">

        <!-- element name -->

        <span class="node">
          &lt;<xsl:value-of select="name()" />&gt;
        </span>
        <xsl:if test="@comment">
          -
          <span class="comment">
            <xsl:value-of select="@comment" />
          </span>
        </xsl:if>

        <!-- attributes and sub-elements -->

        <table width="99%" class="attributes">
          <tr>
            <td>
              <table width="100%" class="attribute">
                <xsl:for-each select="@*">
                  <xsl:if test="name()!='comment'">
                    <tr>
                      <td width="10%" class="attribute">
                        <span class="attributename">
                          <xsl:value-of select="name()" />
                        </span>
                      </td>
                      <td class="attribute">
                        <xsl:value-of select="." />
                      </td>
                    </tr>
                  </xsl:if>
                </xsl:for-each>
              </table>
            </td>
          </tr>
          <xsl:if test="*">
            <tr>
              <td>
                <xsl:choose>
                  <xsl:when test="contains(name(*),'_')">
                    <table width="100%" class="element">
                      <xsl:apply-templates select="*" />
                    </table>
                  </xsl:when>
                  <xsl:otherwise>
                    <table width="100%" class="subnode">
                      <xsl:apply-templates select="*" />
                    </table>
                  </xsl:otherwise>
                </xsl:choose>
              </td>
            </tr>
          </xsl:if>
        </table>
      </td>
    </tr>
  </xsl:template>

</xsl:stylesheet>

