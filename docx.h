/**
 * File              : docx.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 13.08.2025
 * Last Modified Date: 14.08.2025
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/* functions to handle DOCX files */

#ifndef DOCX_H_
#define DOCX_H_

#include <cstdlib>
#include <stdio.h>
#include <zip.h>

/* docx_find_and_replace_text 
 * replace all founded words from searchwords array in 
 * (with argc len) filename docx file  with words from  
 * replaces array (same argc len) and return 0 on success */ 
static int 
docx_find_and_replace_text(
		const char *filename, 
		int argc,
		const char *searchwords[],
		const char *replaces[]);

/* convert image to docx xml string and save it in docx
 * container 
 * with and height are in twips (inches * 20) - 
 * for A4 13011x16838 */
static char *
docx_add_image_and_get_xml(
		const char *filename,
		const char *title,
		int imageid,
		const unsigned char *data,
		size_t len,
		int width, int height);

/* IMPLIMATION */
#include <string.h>
#include <stdint.h>
#include "zip_entry_read.h"
#include "strrep.h"
#include "str.h"
#include "stb_image.h"
#include "stb_image_write.h"

// extract document xml from docx
static char *_docx_get_xml_document(const char *file){
	int err = 0;
	zip_t *zip = zip_open(file, ZIP_RDONLY, &err);
	if (err)
		return NULL;

	void *buffer;
	size_t size;
	if (zip_entry_read(
				zip, "word/document.xml", &buffer, &size))
	{
		zip_close(zip);
		return NULL;
	}

	zip_close(zip);
	return (char *)buffer;
}

// write document xml to docx and return 0 on success
static int _docx_set_xml_document(
		const char *file, const char *xml)
{
	int err = 0;
	zip_t *zip = zip_open(file, 0, &err);
	if (err)
		return -1;

	zip_int64_t index = 
		zip_name_locate(zip, "word/document.xml", 0);
	if (index < 0){
		zip_close(zip);
		return -1;
	}

	zip_source_t *source =
		zip_source_buffer(zip, xml, strlen(xml), 0);
	if (!source){
		zip_close(zip);
		return -1;
	}

	err = zip_file_replace(zip, index, source,
		 	ZIP_FL_OVERWRITE|ZIP_FL_ENC_UTF_8);

	zip_close(zip);
	return err;
}

int 
docx_find_and_replace_text(
		const char *filename, 
		int argc,
		const char *searchwords[],
		const char *replaces[])
{
	int i, err = 0; 
	
	char *xml = _docx_get_xml_document(filename);
	if (!xml)
		return -1;

	for (i = 0; i < argc; ++i) {
		char *newxml = 
			strrep(xml, searchwords[i], replaces[i]);	
		if (!newxml)
			break;

		free(xml);
		xml = newxml;
	}

	err = _docx_set_xml_document(filename, xml);

	free(xml);

	return err;
}

static const char image_template[] = 
		"<w:r>\n"
			"<w:rPr/>\n"
			"<w:drawing>\n"
				"<wp:inline distT=\"0\" distB=\"0\" distL=\"0\" distR=\"0\">\n"
					"<wp:docPr id=\"%d\" name=\"Image%d\" descr=\"\" title=\"%s\"/>\n"
					"<wp:cNvGraphicFramePr>\n"
						"<a:graphicFrameLocks xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" noChangeAspect=\"1\"/>\n"
						"</wp:cNvGraphicFramePr>\n"
						"<a:graphic xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\">\n"
							"<a:graphicData uri=\"http://schemas.openxmlformats.org/drawingml/2006/picture\">\n"
								"<pic:pic xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\">\n"
									"<pic:nvPicPr>\n"
									"<pic:cNvPr id=\"%d\" name=\"Image%d\" descr=\"\" title=\"%s\"/>\n"
									"<pic:cNvPicPr>\n"
										"<a:picLocks noChangeAspect=\"1\" noChangeArrowheads=\"1\"/>\n"
									"</pic:cNvPicPr>\n"
									"</pic:nvPicPr>\n"
									"<pic:blipFill>\n"
										"<a:blip r:embed=\"rId%d\"/>\n"
										"<a:stretch>\n"
											"<a:fillRect/>\n"
										"</a:stretch>\n"
									"</pic:blipFill>\n"
									"<pic:spPr bwMode=\"auto\">\n"
										"<a:xfrm>\n"
											"<a:off x=\"0\" y=\"0\"/>\n"
											"<a:ext cx=\"%d\" cy=\"%d\"/>\n"
										"</a:xfrm>\n"
										"<a:prstGeom prst=\"rect\">\n"
											"<a:avLst/>\n"
										"</a:prstGeom>\n"
									"</pic:spPr>\n"
								"</pic:pic>\n"
							"</a:graphicData>\n"
						"</a:graphic>\n"
					"</wp:inline>\n"
				"</w:drawing>\n"
			"</w:r>\n";

static void 
_docx_stbi_write_func(
		void *context, void *data, int size)
{
	struct str *s = (struct str *)context;
	str_append(s, (char *)data, size);
}

static int _docx_add_image(
		const char *filename, int imageid, 
		const unsigned char *data, size_t len)
{
	char imagename[BUFSIZ];
	snprintf(imagename, BUFSIZ-1,
		 	"word/media/image%d.jpeg", imageid);

	int err = 0;
	zip_t *zip = zip_open(filename, 0, &err);
	if (err){
		return -1;
	}

	int x, y, c;
	stbi_uc *image = stbi_load_from_memory(
			data, len, &x, &y,
		 	&c, 0);
	if (!image){
		zip_close(zip);
		return -1;
	}

	struct str buffer;
	if (str_init(&buffer)){
		return -1;
	}

	if (!stbi_write_jpg_to_func(_docx_stbi_write_func,
		 	&buffer, x, y, c, image, 100))
	{
		stbi_image_free(image);
		zip_close(zip);
		free(buffer.str);
		return -1;
	}
	
	stbi_image_free(image);

	zip_source_t *source =
		zip_source_buffer(zip, buffer.str, buffer.len, 0);
	if (!source){
		free(buffer.str);
		zip_close(zip);
		return -1;
	}
		
	free(buffer.str);

	zip_int64_t index = 
		zip_file_add(zip, imagename, source, 0);
	if (index < 0){
		zip_close(zip);
		return -1;
	}

	zip_close(zip);

	return err;
}

char *
docx_add_image_and_get_xml(
		const char *filename,
		const char *title,
		int imageid,
		const unsigned char *data,
		size_t len,
		int width, int height)
{
	if (_docx_add_image(filename, imageid, data, len))
		return NULL;

	int size = sizeof(image_template) + BUFSIZ;
	
	char *xml = (char *)malloc(size + 1);
	if (!xml){
		perror("malloc");
		return NULL;
	}

	snprintf(xml, len, image_template, 
			imageid, imageid, title, imageid, imageid, 
			title, imageid, width, height);

	return xml;
}

#endif /* ifndef DOCX_H_ */
