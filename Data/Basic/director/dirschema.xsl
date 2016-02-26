<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xs="http://www.w3.org/2001/XMLSchema">

  <!-- filter parameter -->
  
  <xsl:param name="filter" />
  <xsl:param name="sort" />

  <!-- schema definition -->

  <xsl:template match="xs:schema">
    <html>

      <head>
        <title>
          Mission Director Definition
        </title>
        <link rel="stylesheet" href="director.css" type="text/css" />
      </head>

      <body>
        <table width="99%" class="cue">
          <tr>
            <th class="cue">
              <span class="title" >
                Mission Director Definition
                <xsl:if test="$filter">
                  <span class="comment">
                    - Filter:
                    <xsl:value-of select="$filter" />
                  </span>
                </xsl:if>
              </span>
            </th>
          </tr>
          <tr>
            <td>
              <span class="elementtitle">
                Condition definitions
              </span>
              <br />
              <table width="100%" class="element">
                <xsl:apply-templates select="/xs:schema/xs:group">
                  <xsl:with-param name="ref" select="'condition'" />
                </xsl:apply-templates>
              </table>
            </td>
          </tr>
          <tr>
            <td>
              <span class="elementtitle">
                Action definitions
              </span>
              <br />
              <table width="100%" class="element">
                <xsl:apply-templates select="/xs:schema/xs:group">
                  <xsl:with-param name="ref" select="'action'" />
                </xsl:apply-templates>
              </table>
            </td>
          </tr>
          <tr>
            <td>
              <span class="elementtitle">
                Lookup definitions
              </span>
              <br />
              <table width="100%" class="element">
                <xsl:choose>
                  <xsl:when test="$sort='on'">
                    <xsl:apply-templates select="/xs:schema/xs:simpleType">
                      <xsl:with-param name="type" select="'lookup'" />
                      <xsl:sort select="@name" />
                    </xsl:apply-templates>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:apply-templates select="/xs:schema/xs:simpleType">
                      <xsl:with-param name="type" select="'lookup'" />
                    </xsl:apply-templates>
                  </xsl:otherwise>
                </xsl:choose>
              </table>
            </td>
          </tr>
          <tr>
            <td>
              <span class="elementtitle">
                Variable definitions
              </span>
              <br />
              <table width="100%" class="element">
                <xsl:choose>
                  <xsl:when test="$sort='on'">
                    <xsl:apply-templates select="/xs:schema/xs:simpleType">
                      <xsl:with-param name="type" select="'variable'" />
                      <xsl:sort select="@name" />
                    </xsl:apply-templates>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:apply-templates select="/xs:schema/xs:simpleType">
                      <xsl:with-param name="type" select="'variable'" />
                    </xsl:apply-templates>
                  </xsl:otherwise>
                </xsl:choose>
              </table>
            </td>
          </tr>
        </table>
      </body>

    </html>
  </xsl:template>

  <!-- element definition -->

  <xsl:template match="xs:group">
    <xsl:param name="doc" />
    <xsl:param name="ref" />
    <xsl:choose>
      <xsl:when test="@ref">
        <xsl:choose>

          <!-- conditions -->

          <xsl:when test="@ref='condition'">
            <tr>
              <td class="element">
                <span class="node">
                  <span class="comment">
                    list of conditions - mandatory - multiple
                  </span>
                </span>
              </td>
            </tr>
          </xsl:when>

          <!-- actions -->

          <xsl:when test="@ref='action'">
            <tr>
              <td class="element">
                <span class="node">
                  <span class="comment">
                    list of actions - mandatory - multiple
                  </span>
                </span>
              </td>
            </tr>
          </xsl:when>

          <!-- other -->

          <xsl:otherwise>
            <xsl:choose>
              <xsl:when test="xs:annotation/xs:documentation and $doc">
                <xsl:apply-templates select="/xs:schema/xs:group">
                  <xsl:with-param name="doc" select="concat(xs:annotation/xs:documentation,' - ',$doc)" />
                  <xsl:with-param name="ref" select="@ref" />
                </xsl:apply-templates>
              </xsl:when>
              <xsl:when test="xs:annotation/xs:documentation">
                <xsl:apply-templates select="/xs:schema/xs:group">
                  <xsl:with-param name="doc" select="xs:annotation/xs:documentation" />
                  <xsl:with-param name="ref" select="@ref" />
                </xsl:apply-templates>
              </xsl:when>
              <xsl:otherwise>
                <xsl:apply-templates select="/xs:schema/xs:group">
                  <xsl:with-param name="doc" select="$doc" />
                  <xsl:with-param name="ref" select="@ref" />
                </xsl:apply-templates>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:otherwise>
          
        </xsl:choose>
      </xsl:when>
      <xsl:when test="$ref">
        <xsl:if test="@name=$ref">
          <xsl:choose>
            
            <!-- conditions/actions -->

            <xsl:when test="@name='condition' or @name='action'">
              <xsl:choose>
                <xsl:when test="$filter">
                  <xsl:choose>
                    <xsl:when test="$sort='on'">
                      <xsl:apply-templates select="xs:choice/xs:element[contains(@name,$filter) or contains(xs:annotation/xs:documentation,$filter)]">
                        <xsl:sort select="@name" />
                      </xsl:apply-templates>
                    </xsl:when>
                    <xsl:otherwise>
                      <xsl:apply-templates select="xs:choice/xs:element[contains(@name,$filter) or contains(xs:annotation/xs:documentation,$filter)]" />
                    </xsl:otherwise>
                  </xsl:choose>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:choose>
                    <xsl:when test="$sort='on'">
                      <xsl:apply-templates select="xs:choice/xs:element">
                        <xsl:sort select="@name" />
                      </xsl:apply-templates>
                    </xsl:when>
                    <xsl:otherwise>
                      <xsl:apply-templates select="xs:choice/xs:element" />
                    </xsl:otherwise>
                  </xsl:choose>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:when>

            <!-- other -->

            <xsl:otherwise>
              <xsl:choose>
                <xsl:when test="xs:annotation/xs:documentation and $doc">
                  <xsl:apply-templates select="xs:sequence|xs:choice">
                    <xsl:with-param name="doc" select="concat(xs:annotation/xs:documentation,' - ',$doc)" />
                  </xsl:apply-templates>
                </xsl:when>
                <xsl:when test="xs:annotation/xs:documentation">
                  <xsl:apply-templates select="xs:sequence|xs:choice">
                    <xsl:with-param name="doc" select="xs:annotation/xs:documentation" />
                  </xsl:apply-templates>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:apply-templates select="xs:sequence|xs:choice">
                    <xsl:with-param name="doc" select="$doc" />
                  </xsl:apply-templates>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:otherwise>

          </xsl:choose>
        </xsl:if>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xs:element">
    <xsl:param name="doc" />
    <xsl:param name="ref" />
    <xsl:param name="min" />
    <xsl:param name="max" />
    <xsl:choose>

      <!-- recursive array/value structure -->

      <xsl:when test="contains(@ref,'value') and contains(../../../@name,'array') and substring-before(@ref,'value')=substring-before(../../../@name,'array')">
        <tr>
          <td class="element">
            <span class="node">
              &lt;<xsl:value-of select="@ref" />&gt;

              <!-- documentation -->

              <span class="comment">
                <xsl:choose>
                  <xsl:when test="xs:annotation/xs:documentation and $doc">
                    <xsl:value-of select="concat(xs:annotation/xs:documentation,' - ',$doc)" />
                  </xsl:when>
                  <xsl:when test="xs:annotation/xs:documentation">
                    <xsl:value-of select="xs:annotation/xs:documentation" />
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:value-of select="$doc" />
                  </xsl:otherwise>
                </xsl:choose>
                <xsl:if test="@minOccurs='1' or $min='1'">
                  <span class="comment">
                    - mandatory
                  </span>
                </xsl:if>
                <xsl:if test="@maxOccurs='unbounded' or $max='unbounded'">
                  <span class="comment">
                    - multiple
                  </span>
                </xsl:if>
              </span>
              
            </span>
          </td>
        </tr>
      </xsl:when>

      <!-- reference -->

      <xsl:when test="@ref">
        <xsl:choose>
          <xsl:when test="xs:annotation/xs:documentation and $doc">
            <xsl:apply-templates select="/xs:schema/xs:element">
              <xsl:with-param name="doc" select="concat(xs:annotation/xs:documentation,' - ',$doc)" />
              <xsl:with-param name="ref" select="@ref" />
              <xsl:with-param name="min" select="@minOccurs" />
              <xsl:with-param name="max" select="@maxOccurs" />
            </xsl:apply-templates>
          </xsl:when>
          <xsl:when test="xs:annotation/xs:documentation">
            <xsl:apply-templates select="/xs:schema/xs:element">
              <xsl:with-param name="doc" select="xs:annotation/xs:documentation" />
              <xsl:with-param name="ref" select="@ref" />
              <xsl:with-param name="min" select="@minOccurs" />
              <xsl:with-param name="max" select="@maxOccurs" />
            </xsl:apply-templates>
          </xsl:when>
          <xsl:otherwise>
            <xsl:apply-templates select="/xs:schema/xs:element">
              <xsl:with-param name="doc" select="$doc" />
              <xsl:with-param name="ref" select="@ref" />
              <xsl:with-param name="min" select="@minOccurs" />
              <xsl:with-param name="max" select="@maxOccurs" />
            </xsl:apply-templates>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>

      <!-- normal element -->

      <xsl:when test="($ref and @name=$ref) or not($ref)">
        <tr>
          <td class="element">
            <span class="node">
              &lt;<xsl:value-of select="@name" />&gt;

              <!-- documentation -->

              <xsl:if test="$doc or xs:annotation/xs:documentation">
                <span class="comment">
                  <xsl:if test="$doc">
                    <xsl:value-of select="$doc" />
                  </xsl:if>
                  <xsl:if test="$doc and xs:annotation/xs:documentation">
                    -
                  </xsl:if>
                  <xsl:if test="xs:annotation/xs:documentation">
                    <xsl:value-of select="xs:annotation/xs:documentation" />
                  </xsl:if>
                </span>
              </xsl:if>
              <xsl:if test="@minOccurs='1' or $min='1'">
                <span class="comment">
                  - mandatory
                </span>
              </xsl:if>
              <xsl:if test="@maxOccurs='unbounded' or $max='unbounded'">
                <span class="comment">
                  - multiple
                </span>
              </xsl:if>

              <!-- referenced type -->

              <xsl:if test="@type">
                <table width="98%" class="attributes">
                  <xsl:apply-templates select="/xs:schema/xs:complexType">
                    <xsl:with-param name="doc" select="$doc" />
                    <xsl:with-param name="pass" select="'attribute'" />
                    <xsl:with-param name="name" select="@name" />
                    <xsl:with-param name="ref" select="@type" />
                  </xsl:apply-templates>
                </table>
                <table width="98%" class="attributes">
                  <xsl:apply-templates select="/xs:schema/xs:complexType">
                    <xsl:with-param name="doc" select="$doc" />
                    <xsl:with-param name="pass" select="'node'" />
                    <xsl:with-param name="name" select="@name" />
                    <xsl:with-param name="ref" select="@type" />
                  </xsl:apply-templates>
                </table>
              </xsl:if>

              <!-- local type -->

              <xsl:if test="xs:complexType">
                <table width="98%" class="attributes">
                  <xsl:apply-templates select="xs:complexType">
                    <xsl:with-param name="doc" select="$doc" />
                    <xsl:with-param name="pass" select="'attribute'" />
                  </xsl:apply-templates>
                </table>
                <table width="98%" class="attributes">
                  <xsl:apply-templates select="xs:complexType">
                    <xsl:with-param name="doc" select="$doc" />
                    <xsl:with-param name="pass" select="'node'" />
                  </xsl:apply-templates>
                </table>
              </xsl:if>

            </span>
          </td>
        </tr>
      </xsl:when>
      
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xs:complexType">
    <xsl:param name="doc" />
    <xsl:param name="extension" />
    <xsl:param name="pass" />
    <xsl:param name="name" />
    <xsl:param name="ref" />
    <xsl:choose>

      <!-- resolve reference -->

      <xsl:when test="@ref">
        <xsl:apply-templates select="/xs:schema/xs:complexType">
          <xsl:with-param name="doc" select="$doc" />
          <xsl:with-param name="name" select="$name" />
          <xsl:with-param name="ref" select="@ref" />
        </xsl:apply-templates>
      </xsl:when>

      <!-- type definition -->

      <xsl:when test="($ref and @name=$ref) or not($ref)">
        <xsl:choose>

          <!-- nested sub-nodes -->

          <xsl:when test="$extension">

            <!-- attributes -->

            <xsl:if test="$pass='attribute'">
              <xsl:apply-templates select="xs:complexContent/xs:extension">
                <xsl:with-param name="doc" select="$doc" />
                <xsl:with-param name="pass" select="$pass" />
              </xsl:apply-templates>
              <xsl:apply-templates select="xs:attribute|xs:attributeGroup" />
            </xsl:if>

            <!-- sub-nodes -->

            <xsl:if test="$pass='node'">
              <xsl:apply-templates select="xs:complexContent/xs:extension">
                <xsl:with-param name="doc" select="$doc" />
                <xsl:with-param name="pass" select="$pass" />
              </xsl:apply-templates>
              <xsl:apply-templates select="xs:sequence|xs:choice|xs:element|xs:group">
                <xsl:with-param name="doc" select="$doc" />
              </xsl:apply-templates>
            </xsl:if>
            
          </xsl:when>

          <!-- top-level nodes -->

          <xsl:otherwise>

            <!-- attributes -->

            <xsl:if test="$pass='attribute' and (xs:complexContent/xs:extension or xs:attribute or xs:attributeGroup)">
              <tr>
                <td>
                  <table width="100%" class="attribute">
                    <xsl:apply-templates select="xs:complexContent/xs:extension">
                      <xsl:with-param name="doc" select="$doc" />
                      <xsl:with-param name="pass" select="$pass" />
                    </xsl:apply-templates>
                    <xsl:apply-templates select="xs:attribute|xs:attributeGroup" />
                  </table>
                </td>
              </tr>
            </xsl:if>

            <!-- sub-nodes -->

            <xsl:if test="$pass='node' and (xs:complexContent/xs:extension or xs:sequence or xs:choice or xs:element or xs:group)">
              <tr>
                <td>
                  <xsl:choose>
                    <xsl:when test="contains($name,'_') or contains(../@name,'do_') or contains(../@name,'check_')">
                      <table width="100%" class="element">
                        <xsl:apply-templates select="xs:complexContent/xs:extension">
                          <xsl:with-param name="doc" select="$doc" />
                          <xsl:with-param name="pass" select="$pass" />
                        </xsl:apply-templates>
                        <xsl:apply-templates select="xs:sequence|xs:choice|xs:element|xs:group">
                          <xsl:with-param name="doc" select="$doc" />
                        </xsl:apply-templates>
                      </table>
                    </xsl:when>
                    <xsl:otherwise>
                      <table width="100%" class="subnode">
                        <xsl:apply-templates select="xs:complexContent/xs:extension">
                          <xsl:with-param name="doc" select="$doc" />
                          <xsl:with-param name="pass" select="$pass" />
                        </xsl:apply-templates>
                        <xsl:apply-templates select="xs:sequence|xs:choice|xs:element|xs:group">
                          <xsl:with-param name="doc" select="$doc" />
                        </xsl:apply-templates>
                      </table>
                    </xsl:otherwise>
                  </xsl:choose>
                </td>
              </tr>
            </xsl:if>
            
          </xsl:otherwise>
          
        </xsl:choose>
      </xsl:when>
      
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xs:attribute">
    <xsl:param name="doc" />
    <xsl:param name="ref" />
    <xsl:value-of select="@doc" />
    <xsl:choose>

      <!-- attributes to ignore -->

      <!-- xsl:when test="@name='typecode' or substring-after(@name,preceding-sibling::*/@name)='id'" / -->
      
      <!-- resolve reference -->

      <xsl:when test="@ref">
        <xsl:choose>
          <xsl:when test="xs:annotation/xs:documentation and $doc">
            <xsl:apply-templates select="/xs:schema/xs:attribute">
              <xsl:with-param name="doc" select="concat(xs:annotation/xs:documentation,' - ',$doc)" />
              <xsl:with-param name="ref" select="@ref" />
            </xsl:apply-templates>
          </xsl:when>
          <xsl:when test="xs:annotation/xs:documentation">
            <xsl:apply-templates select="/xs:schema/xs:attribute">
              <xsl:with-param name="doc" select="xs:annotation/xs:documentation" />
              <xsl:with-param name="ref" select="@ref" />
            </xsl:apply-templates>
          </xsl:when>
          <xsl:otherwise>
            <xsl:apply-templates select="/xs:schema/xs:attribute">
              <xsl:with-param name="doc" select="$doc" />
              <xsl:with-param name="ref" select="@ref" />
            </xsl:apply-templates>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>

      <!-- attribute definition -->

      <xsl:when test="($ref and @name=$ref) or not($ref)">
        <tr>
          <td width="10%" class="element">
            <span class="attributename">
              <xsl:value-of select="@name" />
            </span>
          </td>
          <td width="10%" class="element">
            <xsl:choose>
              <xsl:when test="contains(@type,'variable')">
                <a href="#{substring-before(@type,'variable')}" class="lookup">
                  <xsl:value-of select="substring-before(@type,'variable')" />
                </a>
              </xsl:when>
              <xsl:when test="contains(@type,'lookup') and substring-after(@name,'lookup')=''">
                <a href="#{substring-before(@type,'lookup')}" class="lookup">
                  <xsl:value-of select="substring-before(@type,'lookup')" />
                </a>
              </xsl:when>
              <xsl:when test="contains(@type,'name')">
                <span class="type">
                  <xsl:value-of select="substring-before(@type,'name')" />
                </span>
              </xsl:when>
              <xsl:when test="@type='text' or @type='expression'">
                <span class="type">
                  <xsl:value-of select="@type" />
                </span>
              </xsl:when>
              <xsl:when test="contains(@type,'xs:')">
                <span class="description">
                  <xsl:value-of select="substring-after(@type,'xs:')" />
                </span>
              </xsl:when>
              <xsl:otherwise>
                <span class="description">
                  <xsl:value-of select="@type" />
                </span>
              </xsl:otherwise>
            </xsl:choose>
          </td>
          <td class="element">
            <xsl:if test="$doc or xs:annotation/xs:documentation">
              <span class="comment">
                <xsl:if test="$doc">
                  <xsl:value-of select="$doc" />
                </xsl:if>
                <xsl:if test="$doc and xs:annotation/xs:documentation">
                  -
                </xsl:if>
                <xsl:if test="xs:annotation/xs:documentation">
                  <xsl:value-of select="xs:annotation/xs:documentation" />
                </xsl:if>
              </span>
            </xsl:if>
          </td>
        </tr>
      </xsl:when>
      
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xs:attributeGroup">
    <xsl:param name="doc" />
    <xsl:param name="ref" />
    <xsl:choose>
      <xsl:when test="@ref">
        <xsl:choose>
          <xsl:when test="xs:annotation/xs:documentation and $doc">
            <xsl:apply-templates select="/xs:schema/xs:attributeGroup">
              <xsl:with-param name="doc" select="concat(xs:annotation/xs:documentation,' - ',$doc)" />
              <xsl:with-param name="ref" select="@ref" />
            </xsl:apply-templates>
          </xsl:when>
          <xsl:when test="xs:annotation/xs:documentation">
            <xsl:apply-templates select="/xs:schema/xs:attributeGroup">
              <xsl:with-param name="doc" select="xs:annotation/xs:documentation" />
              <xsl:with-param name="ref" select="@ref" />
            </xsl:apply-templates>
          </xsl:when>
          <xsl:otherwise>
            <xsl:apply-templates select="/xs:schema/xs:attributeGroup">
              <xsl:with-param name="doc" select="$doc" />
              <xsl:with-param name="ref" select="@ref" />
            </xsl:apply-templates>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:when test="($ref and @name=$ref) or not($ref)">
        <xsl:choose>
          <xsl:when test="xs:annotation/xs:documentation and $doc">
            <xsl:apply-templates select="*">
              <xsl:with-param name="doc" select="concat(xs:annotation/xs:documentation,' - ',$doc)" />
            </xsl:apply-templates>
          </xsl:when>
          <xsl:when test="xs:annotation/xs:documentation">
            <xsl:apply-templates select="*">
              <xsl:with-param name="doc" select="xs:annotation/xs:documentation" />
            </xsl:apply-templates>
          </xsl:when>
          <xsl:otherwise>
            <xsl:apply-templates select="*">
              <xsl:with-param name="doc" select="$doc" />
            </xsl:apply-templates>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xs:extension">
    <xsl:param name="doc" />
    <xsl:param name="pass" />
    <xsl:apply-templates select="/xs:schema/xs:complexType">
      <xsl:with-param name="doc" select="$doc" />
      <xsl:with-param name="extension" select="'true'" />
      <xsl:with-param name="pass" select="$pass" />
      <xsl:with-param name="ref" select="@base" />
    </xsl:apply-templates>
    <xsl:if test="$pass='attribute'">
      <xsl:apply-templates select="*">
        <xsl:with-param name="doc" select="$doc" />
      </xsl:apply-templates>
    </xsl:if>
  </xsl:template>

  <xsl:template match="xs:sequence|xs:choice">
    <xsl:param name="doc" />
    <xsl:apply-templates select="*">
      <xsl:with-param name="doc" select="$doc" />
    </xsl:apply-templates>
  </xsl:template>

  <!-- type definition -->

  <xsl:template match="xs:simpleType">
    <!-- xsl:if test="contains(@name,'lookup') and (contains(@name,$filter) or contains(xs:annotation/xs:documentation,$filter) or not($filter))"-->
    <xsl:param name="type" />
    <xsl:choose>
      <xsl:when test="$type and not($type='lookup') and not(type='variable') and @name=$type">
        <xsl:choose>
          <xsl:when test="xs:restriction">
            <xsl:apply-templates select="xs:restriction" />
          </xsl:when>
          <xsl:when test="xs:union">
            <xsl:if test="xs:union/@memberTypes">
              <xsl:call-template name="splitUnion">
                <xsl:with-param name="union" select="xs:union/@memberTypes" />
              </xsl:call-template>
            </xsl:if>
            <xsl:if test="xs:union/xs:simpleType">
              <xsl:apply-templates select="xs:union/xs:simpleType/xs:restriction" />
            </xsl:if>
          </xsl:when>
        </xsl:choose>
      </xsl:when>
      <xsl:when test="contains(@name,'variable')">
        <xsl:if test="not($type) or $type='variable'">
          <tr>
            <td class="element">
              <span class="lookup">
                <a name="{substring-before(@name,'variable')}" />
                <xsl:value-of select="substring-before(@name,'variable')" />
                <xsl:if test="xs:annotation/xs:documentation">
                  <span class="comment">
                    -
                    <xsl:value-of select="xs:annotation/xs:documentation" />
                  </span>
                </xsl:if>
              </span>
              <table width="98%" class="attributes">
                <tr>
                  <td class="element">
                    <table width="98%" class="attribute">
                      <xsl:choose>
                        <xsl:when test="$sort='on'">
                          <xsl:for-each select="xs:restriction/xs:enumeration">
                            <xsl:sort select="@value" />
                            <tr>
                              <td width="10%" class="element">
                                <span class="value">
                                  <xsl:call-template name="parseVariable">
                                    <xsl:with-param name="variable" select="@value" />
                                  </xsl:call-template>
                                </span>
                              </td>
                              <td width="10%" class="element">
                                <span class="value">
                                  <xsl:value-of select="xs:annotation/xs:appinfo/example/text()" />
                                </span>
                              </td>
                              <td class="element">
                                <xsl:if test="xs:annotation/xs:documentation">
                                  <span class="comment">
                                    <xsl:value-of select="xs:annotation/xs:documentation" />
                                  </span>
                                </xsl:if>
                              </td>
                            </tr>
                          </xsl:for-each>
                        </xsl:when>
                        <xsl:otherwise>
                          <xsl:for-each select="xs:restriction/xs:enumeration">
                            <tr>
                              <td width="10%" class="element">
                                <span class="value">
                                  <xsl:call-template name="parseVariable">
                                    <xsl:with-param name="variable" select="@value" />
                                  </xsl:call-template>
                                </span>
                              </td>
                              <td width="10%" class="element">
                                <span class="value">
                                  <xsl:value-of select="xs:annotation/xs:appinfo/example/text()" />
                                </span>
                              </td>
                              <td class="element">
                                <xsl:if test="xs:annotation/xs:documentation">
                                  <span class="comment">
                                    <xsl:value-of select="xs:annotation/xs:documentation" />
                                  </span>
                                </xsl:if>
                              </td>
                            </tr>
                          </xsl:for-each>
                        </xsl:otherwise>
                      </xsl:choose>
                    </table>
                  </td>
                </tr>
              </table>
            </td>
          </tr>
        </xsl:if>
      </xsl:when>
      <xsl:when test="contains(@name,'lookup') and substring-after(@name,'lookup')=''">
        <xsl:if test="not($type) or $type='lookup'">
          <tr>
            <td class="element">
              <span class="lookup">
                <a name="{substring-before(@name,'lookup')}" />
                <xsl:value-of select="substring-before(@name,'lookup')" />
                <xsl:if test="xs:annotation/xs:documentation">
                  <span class="comment">
                    -
                    <xsl:value-of select="xs:annotation/xs:documentation" />
                  </span>
                </xsl:if>
              </span>
              <table width="98%" class="attributes">
                <tr>
                  <td class="element">
                    <table width="98%" class="attribute">
                      <xsl:choose>
                        <xsl:when test="xs:restriction">
                          <xsl:apply-templates select="xs:restriction" />
                        </xsl:when>
                        <xsl:when test="xs:union">
                          <xsl:if test="xs:union/@memberTypes">
                            <xsl:call-template name="splitUnion">
                              <xsl:with-param name="union" select="xs:union/@memberTypes" />
                            </xsl:call-template>
                          </xsl:if>
                          <xsl:if test="xs:union/xs:simpleType">
                            <xsl:apply-templates select="xs:union/xs:simpleType/xs:restriction" />
                          </xsl:if>
                        </xsl:when>
                      </xsl:choose>
                    </table>
                  </td>
                </tr>
              </table>
            </td>
          </tr>
        </xsl:if>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xs:restriction">
      <xsl:for-each select="xs:enumeration">
        <tr>
          <td width="10%" class="element">
            <span class="value">
              <xsl:value-of select="@value" />
            </span>
          </td>
          <td class="element">
            <xsl:if test="xs:annotation/xs:documentation">
              <span class="comment">
                <xsl:value-of select="xs:annotation/xs:documentation" />
              </span>
            </xsl:if>
          </td>
        </tr>
      </xsl:for-each>
  </xsl:template>

  <xsl:template name="splitUnion">
    <xsl:param name="union" />
    <xsl:choose>
      <xsl:when test="contains($union,' ')">
        <xsl:apply-templates select="/xs:schema/xs:simpleType">
          <xsl:with-param name="type" select="substring-before($union,' ')" />
        </xsl:apply-templates>
        <xsl:call-template name="splitUnion">
          <xsl:with-param name="union" select="substring-after($union,' ')" />
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="/xs:schema/xs:simpleType">
          <xsl:with-param name="type" select="$union" />
        </xsl:apply-templates>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="parseVariable">
    <xsl:param name="variable" />
    <xsl:choose>
      <xsl:when test="contains($variable,'&lt;') and contains(substring-after($variable,'&lt;'),'&gt;')">
        <b>
          <xsl:value-of select="substring-before($variable,'&lt;')" />
        </b>
        <i>
          <xsl:choose>
            <xsl:when test="contains(substring-before(substring-after($variable,'&lt;'),'&gt;'),'lookup') and substring-after(substring-before(substring-after($variable,'&lt;'),'&gt;'),'lookup')=''">
              <a href="#{substring-before(substring-before(substring-after($variable,'&lt;'),'&gt;'),'lookup')}" class="lookup">
                <xsl:value-of select="substring-before(substring-before(substring-after($variable,'&lt;'),'&gt;'),'lookup')" />
              </a>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="substring-before(substring-after($variable,'&lt;'),'&gt;')" />
            </xsl:otherwise>
          </xsl:choose>
        </i>
        <xsl:call-template name="parseVariable">
          <xsl:with-param name="variable" select="substring-after($variable,'&gt;')" />
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <b>
          <xsl:value-of select="$variable" />
        </b>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>

