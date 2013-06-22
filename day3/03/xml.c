#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

struct file {
    char name[128];
    int size;
};

static void 
parseFile(xmlNode * cur_node, struct file *f) {
    xmlNode *child;
    for (child = cur_node; child; child = child->next) {
        if (child->type == XML_ELEMENT_NODE && !strcmp(child->name, "name")) {
            xmlNode *node;
            for (node = child->children; node; node = node->next) {
                if (node->type == XML_TEXT_NODE) {
                    strcpy(f->name, node->content);
                }
            }
        }
        if (child->type == XML_ELEMENT_NODE && !strcmp(child->name, "size")) {
            xmlNode *node;
            for (node = child->children; node; node = node->next) {
                if (node->type == XML_TEXT_NODE) {
                    f->size = atoi(node->content);
                }
            }
        }
    }
}

static void
print_element_names(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!strcmp(cur_node->name, "file")) {
                if (cur_node->children) {
                    struct file f;
                    parseFile(cur_node->children, &f);
                    printf("name: %s, size: %d\n", f.name, f.size);
                }
            }
        }
    }
}

static void
example1Func(const char *filename) {
    xmlDocPtr doc; /* the resulting document tree */

    doc = xmlReadFile(filename, NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse %s\n", filename);
	return;
    }

    xmlNode *root_element = NULL;
    root_element = xmlDocGetRootElement(doc);
    print_element_names(root_element->children);

    xmlFreeDoc(doc);
}

int main(int argc, char **argv) {
    if (argc != 2)
        return(1);

    LIBXML_TEST_VERSION
    example1Func(argv[1]);
    xmlCleanupParser();
    xmlMemoryDump();
    return(0);
}
