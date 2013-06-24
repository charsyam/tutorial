#include <stdio.h>
#include <string.h>
#include <libxml/SAX.h>

int read_xmlfile(FILE *f);
xmlSAXHandler make_sax_handler();

struct file {
    char name[128];
    int size;
};

struct file xmlfile[128];
int fileindex = 0;

static void OnStartElementNs(
    void *ctx,
    const xmlChar *localname,
    const xmlChar *prefix,
    const xmlChar *URI,
    int nb_namespaces,
    const xmlChar **namespaces,
    int nb_attributes,
    int nb_defaulted,
    const xmlChar **attributes
);

static void OnEndElementNs(
    void* ctx,
    const xmlChar* localname,
    const xmlChar* prefix,
    const xmlChar* URI
);

struct state {
    char *tag;
};

static void OnCharacters(void* ctx, const xmlChar * ch, int len);

int main(int argc, char *argv[]) {
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        puts("file open error.");
        exit(1);
    }

    if(read_xmlfile(f)) {
        puts("xml read error.");
        exit(1);
    }

    fclose(f);
    return 0;
}

int read_xmlfile(FILE *f) {
    char chars[1024];
    int res = fread(chars, 1, 4, f);
    if (res <= 0) {
        return 1;
    }

    xmlSAXHandler SAXHander = make_sax_handler();
    struct state parseState;
    memset(&parseState, 0, sizeof(struct state));
 
    xmlParserCtxtPtr ctxt = xmlCreatePushParserCtxt(
        &SAXHander, &parseState, chars, res, NULL
    );

    while ((res = fread(chars, 1, sizeof(chars), f)) > 0) {
        if(xmlParseChunk(ctxt, chars, res, 0)) {
            xmlParserError(ctxt, "xmlParseChunk");
            return 1;
        }
    }
    xmlParseChunk(ctxt, chars, 0, 1);

    int i = 0;
    for (i=0; i < fileindex; i++) {
        printf("name: %s, size: %d\n", xmlfile[i].name, xmlfile[i].size);
    }
    xmlFreeParserCtxt(ctxt);
    xmlCleanupParser();
    return 0;
}

xmlSAXHandler make_sax_handler (){
    xmlSAXHandler SAXHander;

    memset(&SAXHander, 0, sizeof(xmlSAXHandler));

    SAXHander.initialized = XML_SAX2_MAGIC;
    SAXHander.startElementNs = OnStartElementNs;
    SAXHander.endElementNs = OnEndElementNs;
    SAXHander.characters = OnCharacters;

    return SAXHander;
}

static void OnStartElementNs(
    void *ctx,
    const xmlChar *localname,
    const xmlChar *prefix,
    const xmlChar *URI,
    int nb_namespaces,
    const xmlChar **namespaces,
    int nb_attributes,
    int nb_defaulted,
    const xmlChar **attributes
) {
    struct state *parseState = (struct state *)(ctx);
    parseState->tag = localname;
}

static void OnEndElementNs(
    void* ctx,
    const xmlChar* localname,
    const xmlChar* prefix,
    const xmlChar* URI
) {
    if (!strcmp(localname, "file")) {
        fileindex++;
    }

    struct state *parseState = (struct state *)(ctx);
    parseState->tag = "";
/*
    if (!strcmp(parseState->tag, "name")) {
        printf("</%s>\n", parseState->tag);
        free(parseState->data);
        parseState->data =0;
    } 
    else if (!strcmp(parseState->tag, "size")) {
        printf("\n</%s>\n", parseState->tag);
        free(parseState->data);
        parseState->data =0;
    }
    parseState->tag = "";
*/
}

static void OnCharacters(void *ctx, const xmlChar *ch, int len) {
    struct state *parseState = (struct state *)ctx;

    if (!strcmp(parseState->tag, "name")) {
        fprintf(stderr, "%d\n", fileindex);
        strncpy(xmlfile[fileindex].name, (const char *)ch, len);
        xmlfile[fileindex].name[len] = 0;
    }
    else if (!strcmp(parseState->tag, "size")) {
        char buf[1024];
        strncpy(buf, (const char *)ch, len);
        buf[len] = 0;
        xmlfile[fileindex].size = atoi(buf);
    }
}

