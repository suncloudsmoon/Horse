// This file was automatically generated by the horse compiler
#include "compl.h"
 void startCompiler(string_t *directory, string_t *inputFilename);
 compiler_t* compiler_init(string_t* directory, FILE *inputFile);
 void compiler_free(compiler_t *com);
 class_t* class_new(string_t *name, string_t *includeStatements);
 void vTable_new(string_t *returnType, string_t *name, list_t *parameters, vTable_t *vTable);
 void vTable_delete(vTable_t *vTable);
 void ignition(compiler_t *com);
 void readAllLines(compiler_t *com);
static  bool readLine(FILE *stream, string_t *line) ;
 void parse(compiler_t *com);
static  list_t* split(char delimiter, string_t *line) ;
static  int lex_find(string_t *line, char *format, ...) ;
static  bool isSpecialCharacter(char alpha) ;
 void compile(compiler_t *com);
static  void handleForLoop(string_t *line, string_t *dest) ;
static  void handleFunction(string_t *contextLine, class_t *currentClass, compiler_t *com, string_t *dest) ;
static  vTable_t* functionToVTable(string_t *returnType, string_t *functName, string_t *functBody) ;
static  void handlePrivateFunction(string_t *contextLine, class_t *currentClass, compiler_t *com, string_t *dest) ;
static  void addFunctionHeader(string_t *functionBody, compiler_t *com) ;
static  void addDefinitionToHeader(string_t *definition, compiler_t *com) ;
 void writeToFile(compiler_t *com);
static  void writeToDefFile(string_t *directory, list_t *definitionsList) ;
static  void writeToHeaderFile(string_t *fullHeaderPath, class_t *currentClass) ;
static  void getClassStruct(class_t *currentClass, string_t *cls) ;
static  void paramListToStr(list_t *params, string_t *dest) ;
static  void writeToSourceFile(string_t *fullSourcePath, class_t *currentClass) ;
 list_t* list_init();
static  list_t* custom_list_init(size_t mallocSize) ;
 void list_add(void *item, list_t *list);
 void list_remove(int index, list_t *list);
 void list_complete_remove(void (*indivfree)(void*), int index, list_t *list);
 void list_clear(list_t *list);
 bool list_equals(void *destComp, int index, bool (*equalsComparator)(void*, void*), list_t *list);
 bool list_contains(void *destComp, bool (*equalsComparator)(void*, void*), list_t *list);
 void list_serialize(void (*indiv)(void*, FILE*), FILE *stream, list_t *list);
 list_t* list_deserialize(void* (*indivreverse)(FILE*), FILE *stream);
 void list_free(list_t *list);
 void list_complete_free(void (*indivfree)(void*), list_t *list);
static  void list_meminspector(size_t addNum, list_t *subject) ;
 string_t* string_init();
static  string_t* custom_string_init(size_t allocationSize) ;
 string_t* string_copyvalueof(char *src);
 string_t* string_copyvalueof_s(string_t *src);
 void string_printf(string_t *dest, char *format, ...);
 void string_append(string_t *dest, char *src);
 void string_append_s(string_t *dest, string_t *src);
 void string_appendchar(string_t *dest, char letter);
 bool string_isempty(string_t *str);
 int string_indexof_s(string_t *src, char *stopSign);
 int string_lastindexof_s(string_t *src, char *stopSign);
 string_t** string_split(char delimiter, string_t *src);
 bool string_equals(string_t *dest, const char *src);
 bool string_equals_s(string_t *dest, string_t *src);
 bool string_equalsignorecase(string_t *dest, const char *src);
 bool string_equalsignorecase_s(string_t *dest, string_t *src);
 bool string_startswith_s(string_t *src, string_t *search);
 bool string_startswith(string_t *src, const char *search);
 string_t* string_substring_s(int startIndex, int endIndex, string_t *src);
 void string_tolowercase_s(string_t *dest);
 bool string_serialize(string_t *src, FILE *stream);
 string_t* string_deserialize(FILE *stream);
 void string_reset(string_t *dest);
 void string_free(void *dest);
static  void string_meminspection(size_t addNum, string_t *subject) ;
 void throw_exception(exception e, int lineNum, char *message, ...);
 int main(int argc, char **argv) {
clock_t start = clock(); // Start the stopwatch
string_t *inputFilename, *directory;
if ( argc >= 4 ) {
printf("[Debug] Successfully accessing the command line arguments!\n");
int i = 1;
directory = string_copyvalueof(argv[i]);
while ( i < argc - 1 ) {
inputFilename = string_copyvalueof(argv[i + 1]);
startCompiler(directory, inputFilename);
string_free(inputFilename);
i += 3;
}
} else {
directory = string_copyvalueof("src");
inputFilename = string_copyvalueof("src/compl.hr");
startCompiler(directory, inputFilename);
string_free(inputFilename);
}
clock_t end = clock(); // End the stopwatch
double time_spent = (double) (end - start) / CLOCKS_PER_SEC;
printf("[main] Program Benchmark: %.2f second(s)\n", time_spent);
string_t *dest = string_init();
string_t *some = string_init();
string_printf(some, "if(wow)");
lex_find(some, "if(%)", dest);
printf("[main] lex stuff: %s\n", dest->text);
return 0;
}
 void startCompiler(string_t *directory, string_t *inputFilename) {
FILE *input = fopen(inputFilename->text, "r");
compiler_t *com = compiler_init(directory, input);
ignition(com);
compiler_free(com);
fclose(input);
return;
}
 compiler_t* compiler_init(string_t* directory, FILE *inputFile) {
compiler_t *com = malloc(sizeof(compiler_t));
com->allLines = list_init();
com->cleanedLines = list_init();
com->parsedLines = list_init();
com->compiledLines = list_init();
com->definitions = list_init();
com->includeStatements = string_init();
com->classes = list_init();
com->directory = directory;
com->inputFile = inputFile;
com->scope = 0;
com->currentClass = -1; // currently in no class
return com;
}
 void compiler_free(compiler_t *com) {
list_complete_free(&string_free, com->allLines);
list_complete_free(&string_free, com->parsedLines);
list_complete_free(&string_free, com->compiledLines);
free(com);
return;
}
 class_t* class_new(string_t *name, string_t *includeStatements) {
class_t *newClass = malloc(sizeof(class_t));
newClass->name = name;
newClass->globalVariables = list_init();
newClass->vTableList = list_init();
newClass->includeStatements = includeStatements;
newClass->definitions = list_init();
newClass->prototypes = list_init();
newClass->restOfLines = list_init();
return newClass;
}
 void vTable_new(string_t *returnType, string_t *name, list_t *parameters, vTable_t *vTable) {
vTable->returnType = string_copyvalueof_s(returnType);
vTable->name = string_copyvalueof_s(name);
vTable->parameters = parameters; // TODO: fix this because it does not do copyvalueof()
return;
}
 void vTable_delete(vTable_t *vTable) {
string_free(vTable->returnType);
string_free(vTable->name);
list_complete_free(&string_free, vTable->parameters);
return;
}
 void ignition(compiler_t *com) {
readAllLines(com);
parse(com);
compile(com);
writeToFile(com);
return;
}
 void readAllLines(compiler_t *com) {
string_t *line = string_init();
while ( !readLine(com->inputFile, line) ) {
list_add(string_copyvalueof_s(line), com->allLines);
string_reset(line);
}
string_free(line);
return;
}
static  bool readLine(FILE *stream, string_t *line)  {
char letter;
while ( (letter = fgetc(stream)) != EOF && letter != '\n' ) {
string_appendchar(line, letter);
}
return letter == EOF;
}
 void parse(compiler_t *com) {
for (num  i =  0 ;  i <  (int)com->allLines->data_length ;  i++) {
string_t *existingLine = (string_t*) com->allLines->data[i];
int firstLetter = strcspn(existingLine->text,
"!\"#$%&\'()*+,-.//0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
string_t *initialClean = string_substring_s(firstLetter,
existingLine->text_length, existingLine);
list_add(initialClean, com->cleanedLines);		
list_add(split(' ', initialClean), com->parsedLines);
}
return;
}
static  list_t* split(char delimiter, string_t *line)  {
list_t *output = list_init();
string_t *temp = string_init();
bool isSpecial = false;
for (num  i =  0 ;  i <  (int)line->text_length ;  i++) {
char alpha = line->text[i];
if ( isSpecialCharacter(alpha) ) {
isSpecial = !isSpecial;
} else if ( !isSpecial && alpha == delimiter ) {
list_add(string_copyvalueof_s(temp), output);
string_reset(temp);
} else {
string_appendchar(temp, alpha);
}
}
list_add(temp, output);
return output;
}
static  int lex_find(string_t *line, char *format, ...)  {
size_t formatLength = strlen(format);
bool isSpecial = false;
va_list args;
va_start(args, format);
string_t *formatToLine = string_copyvalueof(format);
list_t *output = split('%', formatToLine);
printf("[lex_find] output length: %d\n", output->data_length);
for (num  i =  0 ;  i <  output->data_length - 1 ;  i++) {
string_t *arg = (string_t *) va_arg(args, string_t*);
string_t *first = (string_t*) output->data[i];
string_t *second = (string_t*) output->data[i+1];
int firstIndex = string_indexof_s(line, first->text);
int secondIndex = string_lastindexof_s(line, second->text);
printf("[lex_find] Index 1,2: %d, %d\n", firstIndex, secondIndex);
string_t *splitStuff = string_substring_s(firstIndex + first->text_length, secondIndex, line);
string_printf(arg, "%s", splitStuff->text);
string_free(first);
string_free(second);
string_free(splitStuff);
}
va_end(args);
return -1;
}
static  bool isSpecialCharacter(char alpha)  {
return alpha == '"' || alpha == '\'';
}
 void compile(compiler_t *com) {
list_add(string_copyvalueof("#define num long long int"), com->definitions);
string_t *parsed;
for (num  i =  0 ;  i <  (int) com->parsedLines->data_length ;  i++) {
string_t *line = (string_t*) com->cleanedLines->data[i];
if ( string_startswith(line, doubleSlashComment) || string_isempty(line) ) {
continue;
}
list_t *tokens = (list_t *) com->parsedLines->data[i];
string_t *firstToken = (string_t*) tokens->data[0];
if ( string_equals(firstToken, classIdentifier) ) {
list_add(class_new(string_copyvalueof_s((string_t*) tokens->data[1]), string_copyvalueof_s(com->includeStatements)), com->classes);
string_reset(com->includeStatements);
com->currentClass++;
continue;
} else if ( string_equals(firstToken, functionIdentifier) ) {
parsed = string_init();
class_t *currentClass = (class_t*) com->classes->data[com->currentClass];
handleFunction(line, currentClass, com, parsed);
com->scope++;
} else if ( string_startswith(line, privateFunctionIdentifier) ) {
parsed = string_init();
class_t *currentClass = (class_t*) com->classes->data[com->currentClass];
handlePrivateFunction(line, currentClass, com, parsed);
com->scope++;
} else if ( string_equals(firstToken, returnIdentifier) ) {
parsed = string_init();
if ( com->scope == 1 ) {
string_printf(parsed, "%s;\n}", line->text);
list_add(parsed, ((class_t*) com->classes->data[com->currentClass])->restOfLines);
com->scope--;
continue;
} else {
string_printf(parsed, "%s", line->text);
}
} else if ( string_startswith(line, endIdentifier) ) {
parsed = string_copyvalueof("}");
list_add(parsed, ((class_t*) com->classes->data[com->currentClass])->restOfLines);
com->scope--;
continue;
} else if ( string_startswith(line, ifIdentifier) ) {
parsed = string_init();
string_t *middle = string_substring_s(strlen(ifIdentifier), line->text_length - strlen(thenKeyword), line);
string_printf(parsed, "if (%s) {", middle->text);
com->scope++;
} else if ( string_startswith(line, elseIfIdentifier) ) {
parsed = string_init();
string_t *middle = string_substring_s(strlen(elseIfIdentifier), line->text_length - strlen(thenKeyword), line);
string_printf(parsed, "} else if (%s) {", middle->text);
} else if ( string_startswith(line, elseIdentifier) ) {
parsed = string_init();
string_append(parsed, "} else {");
} else if ( string_startswith(line, forIdentifier) ) {
parsed = string_init();
handleForLoop(line, parsed);
com->scope++;
} else if ( string_startswith(line, whileIdentifier) ) {
parsed = string_init();
string_t *condition = string_substring_s(strlen(whileIdentifier), string_indexof_s(line, "do"), line);
string_printf(parsed, "while (%s) {", condition->text);
com->scope++;
} else if ( string_startswith(line, importIdentifier) ) {
string_t *includeStatement = string_init();
string_t *importItem = string_substring_s(strlen(importIdentifier) + 1, line->text_length, line);
if ( string_startswith(importItem, "basics") ) {
string_append(includeStatement, "#include <stdio.h>\n#include <stdlib.h>\n#include <stdbool.h>\n");
string_printf(includeStatement, "#include \"__%s__.h\"", defFileName);
} else {
string_printf(includeStatement, "#include \"%s.h\"", importItem->text);	
}
string_append_s(com->includeStatements, includeStatement);
string_append(com->includeStatements, "\n");
string_free(includeStatement);
string_free(importItem);
continue; // don't add it to the restOfLines list
} else {
parsed = line;
}
if ( com->classes->data_length > 0 && com->scope > 0 ) {
list_add(parsed, ((class_t*) com->classes->data[com->currentClass])->restOfLines);
} else if ( com->classes->data_length > 0 && com->scope <= 0 ) {
addDefinitionToHeader(parsed, com);
}
}
return;
}
static  void handleForLoop(string_t *line, string_t *dest)  {
string_t *startPoint = string_substring_s(strlen(forIdentifier), string_indexof_s(line, toKeyword), line);
string_t *endPoint = string_substring_s(string_indexof_s(line, toKeyword) + strlen(toKeyword), string_indexof_s(line, doKeyword), line);
string_t *lineAfterDo = string_substring_s(string_indexof_s(line, doKeyword) + strlen(doKeyword), line->text_length, line);
string_t *conditionName = string_substring_s(0, string_indexof_s(lineAfterDo, "+"), lineAfterDo);
string_t *incrementValue = string_substring_s(string_indexof_s(lineAfterDo, "+"), lineAfterDo->text_length, lineAfterDo);
string_printf(dest, "for (num %s = %s; %s < %s; %s%s) {", conditionName->text, startPoint->text, conditionName->text, endPoint->text, conditionName->text, incrementValue->text);
string_free(startPoint);
string_free(endPoint);
string_free(lineAfterDo);
string_free(conditionName);
string_free(incrementValue);
return;
}
static  void handleFunction(string_t *contextLine, class_t *currentClass, compiler_t *com, string_t *dest)  {
string_t *functName = string_substring_s(strlen(functionIdentifier) + 1, string_indexof_s(contextLine, "("), contextLine);
string_t *functBody = string_substring_s(string_indexof_s(contextLine, "(") + 1, string_lastindexof_s(contextLine, ")"), contextLine);
string_t *returnType = string_substring_s(string_lastindexof_s(contextLine, "returns") + strlen("returns"), contextLine->text_length, contextLine);
string_printf(dest, "%s %s(%s)", returnType->text, functName->text, functBody->text);
if ( !string_equals(functName, "main") ) {
addFunctionHeader(dest, com); // Add function header thing "void foo()" to the top of the source file
}
string_append(dest, " {"); // complete the function header to be added to source in the future
list_add(functionToVTable(returnType, functName, functBody), currentClass->vTableList);
string_free(functName);
string_free(functBody);
string_free(returnType);
return;
}
static  vTable_t* functionToVTable(string_t *returnType, string_t *functName, string_t *functBody)  {
list_t *parameters = split(',', functBody);
vTable_t *vTable = malloc(sizeof(vTable_t));
vTable_new(returnType, functName, parameters, vTable);
return vTable;
}
static  void handlePrivateFunction(string_t *contextLine, class_t *currentClass, compiler_t *com, string_t *dest)  {
string_t *functionName = string_substring_s(strlen(privateFunctionIdentifier) + 1, string_indexof_s(contextLine, "("), contextLine);
string_t *functionBody = string_substring_s(strlen(privateFunctionIdentifier) + 1, string_indexof_s(contextLine, "returns"), contextLine);
string_t *returnType = string_substring_s(string_indexof_s(contextLine, "returns") + strlen("returns"), contextLine->text_length, contextLine);
string_printf(dest, "static %s %s", returnType->text, functionBody->text);
addFunctionHeader(dest, com);
string_append(dest, " {");
string_free(functionName);
string_free(functionBody);
string_free(returnType);
return;
}
static  void addFunctionHeader(string_t *functionBody, compiler_t *com)  {
string_t *funct = string_init();
string_printf(funct, "%s;", functionBody->text);
list_add(funct, ((class_t*)com->classes->data[com->currentClass])->prototypes);
return;
}
static  void addDefinitionToHeader(string_t *definition, compiler_t *com)  {
list_add(definition, ((class_t*) com->classes->data[com->currentClass])->definitions);
return;
}
 void writeToFile(compiler_t *com) {
writeToDefFile(com->directory, com->definitions);
for (num  i =  0 ;  i <  com->classes->data_length ;  i++) {
class_t *currentClass = (class_t*) com->classes->data[i];
string_t *fullHeaderPath = string_init();
string_t *fullSourcePath = string_init();
string_printf(fullHeaderPath, "src/%s.h", currentClass->name->text);
string_printf(fullSourcePath, "src/%s.c", currentClass->name->text);
writeToHeaderFile(fullHeaderPath, currentClass);
writeToSourceFile(fullSourcePath, currentClass);
string_free(fullHeaderPath);
string_free(fullSourcePath);
}
return;
}
static  void writeToDefFile(string_t *directory, list_t *definitionsList)  {
string_t *fullDefPath = string_init();	
string_printf(fullDefPath, "%s/__%s__.h", directory->text, defFileName);
FILE *definitions = fopen(fullDefPath->text, "w");
fprintf(definitions, "// This file was automatically generated by the horse compiler\n");
fprintf(definitions, "#ifndef %s_H_\n", defFileName);
fprintf(definitions, "#define %s_H_\n", defFileName);
for (num  i =  0 ;  i <  definitionsList->data_length ;  i++) {
fprintf(definitions, "%s\n", ((string_t *) definitionsList->data[i])->text);
}
fprintf(definitions, "#endif\n");
string_free(fullDefPath);
fclose(definitions);
return;
}
static  void writeToHeaderFile(string_t *fullHeaderPath, class_t *currentClass)  {
FILE *headerFile = fopen(fullHeaderPath->text, "w");
fprintf(headerFile, "// This file was automatically generated by the horse compiler\n");
fprintf(headerFile, "#ifndef %s_H_\n", currentClass->name->text);
fprintf(headerFile, "#define %s_H_\n", currentClass->name->text);
fprintf(headerFile, "#include \"__%s__.h\"\n", defFileName); // core definitions of Horse
fprintf(headerFile, "%s\n", currentClass->includeStatements->text);
for (num  j =  0 ;  j <  currentClass->definitions->data_length ;  j++) {
fprintf(headerFile, "%s\n", ((string_t*) currentClass->definitions->data[j])->text);
}
string_t *classStruct = string_init();
getClassStruct(currentClass, classStruct);
fprintf(headerFile, "%s\n", classStruct->text);
fprintf(headerFile, "#endif\n");
fclose(headerFile);
return;
}
static  void getClassStruct(class_t *currentClass, string_t *cls)  {
string_printf(cls, "typedef struct %s %s;\n", currentClass->name->text, currentClass->name->text);
string_printf(cls, "struct %s {\n", currentClass->name->text);
for (num  i =  0 ;  i <  currentClass->vTableList->data_length ;  i++) {
vTable_t *vTable = (vTable_t*) currentClass->vTableList->data[i];
string_t *parametersStr = string_init();
paramListToStr(vTable->parameters, parametersStr);
string_printf(cls, "%s (*%s) (%s);\n", vTable->returnType->text, vTable->name->text, parametersStr->text);
string_free(parametersStr);
}
string_printf(cls, "};\n");
return;
}
static  void paramListToStr(list_t *params, string_t *dest)  {
for (num  i =  0 ;  i <  params->data_length ;  i++) {
string_t *param = (string_t*) params->data[i];
string_append_s(dest, param);
if ( i < params->data_length - 1 ) {
string_append(dest, ",");
}
}
return;
}
static  void writeToSourceFile(string_t *fullSourcePath, class_t *currentClass)  {
FILE *sourceFile = fopen(fullSourcePath->text, "w");
fprintf(sourceFile, "// This file was automatically generated by the horse compiler\n");
fprintf(sourceFile, "#include \"%s.h\"\n", currentClass->name->text);
for (num  j =  0 ;  j <  currentClass->prototypes->data_length ;  j++) {
fprintf(sourceFile, "%s\n", ((string_t*)currentClass->prototypes->data[j])->text);
}
for (num  j =  0 ;  j <  currentClass->restOfLines->data_length ;  j++) {
fprintf(sourceFile, "%s\n", ((string_t*) currentClass->restOfLines->data[j])->text);	
fprintf(stdout, "[writeToSourceFile] writing: %s\n", ((string_t*) currentClass->restOfLines->data[j])->text);		
}
fclose(sourceFile);
return;
}
 list_t* list_init() {
list_t *list = malloc(sizeof(list_t));
list->data = (void**) malloc(LIST_MANAGER_ALLOC_SIZE * sizeof(void*));
list->data_length = 0;
list->data_allocated_length = LIST_MANAGER_ALLOC_SIZE;
return list;
}
static  list_t* custom_list_init(size_t mallocSize)  {
list_t *list = malloc(sizeof(list_t));
list->data = (void**) malloc(mallocSize * sizeof(void*));
list->data_length = 0;
list->data_allocated_length = mallocSize;
return list;
}
 void list_add(void *item, list_t *list) {
list_meminspector(1, list);
list->data[list->data_length] = item;
list->data_length++;
return;
}
 void list_remove(int index, list_t *list) {
unsigned bytes = sizeof(void*) * (list->data_allocated_length - index - 1);
memmove(&list->data[index], &list->data[index+1], bytes);
list->data_length--;
return;
}
 void list_complete_remove(void (*indivfree)(void*), int index, list_t *list) {
(*indivfree)(list->data[index]); // frees it from the respective free method for the unknown type data
list_remove(index, list);
return;
}
 void list_clear(list_t *list) {
list->data_length = 0;
return;
}
 bool list_equals(void *destComp, int index, bool (*equalsComparator)(void*, void*), list_t *list) {
if ( index < 0 || index >= (int)list->data_length ) {
throw_exception(INDEX_OUT_OF_BOUNDS_EXCEPTION, -1,
"Tried to access a list in index %d that was out of bounds!",
index);
}
return (*equalsComparator)(destComp, list->data[index]);;
}
 bool list_contains(void *destComp, bool (*equalsComparator)(void*, void*), list_t *list) {
for (num  i =  0 ;  i <  (int) list->data_length ;  i++) {
if ( (*equalsComparator)(destComp, list->data[i]) ) {
return true;
}
}
return false;
}
 void list_serialize(void (*indiv)(void*, FILE*), FILE *stream, list_t *list) {
fwrite(&list->data_length, sizeof(list->data_length), 1, stream);
for (num  i =  0 ;  i <  (int)list->data_length ;  i++) {
(*indiv)(list->data[i], stream);
}
return;
}
 list_t* list_deserialize(void* (*indivreverse)(FILE*), FILE *stream) {
int arrayLength;
fread(&arrayLength, sizeof(int), 1, stream);
list_t *list = custom_list_init(arrayLength);
for (num  i =  0 ;  i <  arrayLength ;  i++) {
list_add((*indivreverse)(stream), list);
}
return list;
}
 void list_free(list_t *list) {
free(list->data);
free(list);
return;
}
 void list_complete_free(void (*indivfree)(void*), list_t *list) {
for (num  i =  0 ;  i <  (int)list->data_length ;  i++) {
(*indivfree)(list->data[i]);
}
list_free(list);
return;
}
static  void list_meminspector(size_t addNum, list_t *subject)  {
if ( subject->data_length + addNum >= subject->data_allocated_length ) {
size_t newSize = 1.5 * subject->data_allocated_length + addNum;
void **new_ptr = (void**) realloc(subject->data,
newSize * sizeof(void*));
if ( new_ptr == NULL ) {
throw_exception(NULL_POINTER_EXCEPTION, -1,
"Unable to allocate memory for list with length %d!",
subject->data_length);
}
subject->data = new_ptr;
subject->data_allocated_length = newSize;
}
return;
}
 string_t* string_init() {
string_t *str = malloc(sizeof(string_t));
str->text = malloc(STRING_ALLOCATION_SIZE * sizeof(char));
str->text[0] = '\0';
str->text_length = 0;
str->text_allocated_length = STRING_ALLOCATION_SIZE;
return str;
}
static  string_t* custom_string_init(size_t allocationSize)  {
string_t *str = malloc(sizeof(string_t));
str->text = malloc(allocationSize);
str->text[0] = '\0';
str->text_length = 0;
str->text_allocated_length = allocationSize;
return str;
}
 string_t* string_copyvalueof(char *src) {
size_t srcLength = strlen(src);
string_t *dest = malloc(sizeof(string_t));
dest->text = strdup(src);
dest->text_length = srcLength;
dest->text_allocated_length = srcLength + 1;
return dest;
}
 string_t* string_copyvalueof_s(string_t *src) {
string_t *dest = malloc(sizeof(string_t));
dest->text = strdup(src->text);
dest->text_length = src->text_length;
dest->text_allocated_length = src->text_length + 1;
return dest;
}
 void string_printf(string_t *dest, char *format, ...) {
va_list args;
va_start(args, format);
int argsLength = strlen(format);
for (num  i =  0 ;  i <  argsLength ;  i++) {
char first = format[i];
char second = (i + 1 < argsLength) ? format[i+1] : '\0';
char *arg;
if ( first == '%' && second == 's' ) {
arg = va_arg(args, char*);
i++;
} else {
arg = malloc(2 * sizeof(char));
arg[0] = first;
arg[1] = '\0';
}
int argLength = strlen(arg);
string_meminspection(argLength, dest);
strncat(dest->text, arg, argLength);
dest->text_length += argLength;
}
va_end(args);
return;
}
 void string_append(string_t *dest, char *src) {
int srcLength = strlen(src);
string_meminspection(srcLength, dest);
strncat(dest->text, src, srcLength);
dest->text_length += srcLength;
return;
}
 void string_append_s(string_t *dest, string_t *src) {
string_meminspection(src->text_length, dest);
strncat(dest->text, src->text, src->text_length);
dest->text_length += src->text_length;
return;
}
 void string_appendchar(string_t *dest, char letter) {
string_meminspection(1, dest);
char text[2];
text[0] = letter;
text[1] = '\0';
strcat(dest->text, text);
dest->text_length++;
return;
}
 bool string_isempty(string_t *str) {
if ( str->text_length == 0 ) {
return true;
} else {
for (num  i =  0 ;  i <  str->text_length ;  i++) {
if ( !isspace(str->text[i]) ) {
return false;
}
}
}
return true;
}
 int string_indexof_s(string_t *src, char *stopSign) {
int stopSignLength = strlen(stopSign);
bool found = true;
for (num  i =  0 ;  i <  (int) src->text_length - stopSignLength + 1 ;  i++) {
for (num  j =  0 ;  j <  stopSignLength ;  j++) {
if ( src->text[i+j] != stopSign[j] ) {
found = false;
break;
}
}
if ( found ) {
return i;
} else {
found = true;
}
}
return -1;
}
 int string_lastindexof_s(string_t *src, char *stopSign) {
int stopSignLength = strlen(stopSign);
int lastIndexFound = -1;
bool found = true;
for (num  i =  0 ;  i <  src->text_length - stopSignLength + 1 ;  i++) {
for (num  j =  0 ;  j <  stopSignLength ;  j++) {
if ( src->text[i+j] != stopSign[j] ) {
found = false;
break;
}
}
if ( found ) {
lastIndexFound = i;
found = false;
} else {
found = true;
}
}
return lastIndexFound;
}
 string_t** string_split(char delimiter, string_t *src) {
if ( src->text_length <= 2 ) {
throw_exception(INDEX_OUT_OF_BOUNDS_EXCEPTION, -1,
"Unable to substring a string with %d length!",
src->text_length);
}
char delimiterText[2];
delimiterText[0] = delimiter;
delimiterText[1] = '\0';
int splitIndex = strcspn(src->text, delimiterText);
if ( splitIndex == (int)src->text_length ) {
return NULL;
}
string_t **strList = malloc(2 * sizeof(string_t));
strList[0] = custom_string_init(src->text_length / 2);
strList[1] = custom_string_init(src->text_length / 2);
for (num  i =  0 ;  i <  splitIndex ;  i++) {
string_appendchar(strList[0], src->text[i]);
}
for (num  i =  0 ;  i <  (int) src->text_length ;  i++) {
string_appendchar(strList[1], src->text[(splitIndex + 1) + i]);
}
return strList;
}
 bool string_equals(string_t *dest, const char *src) {
unsigned srcLength = strlen(src);
if ( dest->text_length != srcLength ) {
return false;
} else {
return strncmp(dest->text, src, srcLength) == 0;
}
}
 bool string_equals_s(string_t *dest, string_t *src) {
if ( dest->text_length != src->text_length ) {
return false;
} else {
return strncmp(dest->text, src->text, src->text_length) == 0;
}
}
 bool string_equalsignorecase(string_t *dest, const char *src) {
if ( dest->text_length != strlen(src) ) {
return false;
} else {
for (num  i =  0 ;  i <  (int)dest->text_length ;  i++) {
if ( tolower(dest->text[i]) != tolower(src[i]) ) {
return false;
}
}
return true;
}
}
 bool string_equalsignorecase_s(string_t *dest, string_t *src) {
if ( dest->text_length != src->text_length ) {
return false;
} else {
for (num  i =  0 ;  i <  (int)dest->text_length ;  i++) {
if ( (tolower(dest->text[i]) != tolower(src->text[i])) ) {
return false;
}
}
return true;
}
}
 bool string_startswith_s(string_t *src, string_t *search) {
if ( search->text_length > src->text_length ) {
return false;
} else {
for (num  i =  0 ;  i <  (int)search->text_length ;  i++) {
if ( src->text[i] != search->text[i] ) {
return false;
}
}
}
return true;
}
 bool string_startswith(string_t *src, const char *search) {
unsigned searchLength = strlen(search);
if ( searchLength > src->text_length ) {
return false;
}
for (num  i =  0 ;  i <  searchLength ;  i++) {
if ( src->text[i] != search[i] ) {
return false;
}
}
return true;
}
 string_t* string_substring_s(int startIndex, int endIndex, string_t *src) {
size_t totalAppend = endIndex - startIndex; // 1 - 0
if ( totalAppend > src->text_length ) {
throw_exception(INDEX_OUT_OF_BOUNDS_EXCEPTION, -1,
"Unable to substring a string starting from index %d and ending %d",
startIndex, endIndex);
}
string_t *newStr = custom_string_init(totalAppend + 2);
strncpy(newStr->text, src->text + startIndex, totalAppend);
newStr->text[totalAppend] = '\0';
newStr->text_length = totalAppend;
return newStr;
}
 void string_tolowercase_s(string_t *dest) {
for (num  i =  0 ;  i <  (int)dest->text_length ;  i++) {
dest->text[i] = tolower(dest->text[i]);
}
return;
}
 bool string_serialize(string_t *src, FILE *stream) {
fwrite(&src->text_length, sizeof(int), 1, stream);
fwrite(src->text, sizeof(char), src->text_length, stream);
return true;
}
 string_t* string_deserialize(FILE *stream) {
int textLength;
fread(&textLength, sizeof(int), 1, stream);
string_t *str = custom_string_init(textLength + STRING_ALLOCATION_SIZE);
fread(str->text, sizeof(char), textLength, stream);
str->text_length = textLength;
return str;
}
 void string_reset(string_t *dest) {
dest->text[0] = '\0';
dest->text_length = 0;
return;
}
 void string_free(void *dest) {
free(((string_t*) dest)->text);
free(((string_t*) dest));
return;
}
static  void string_meminspection(size_t addNum, string_t *subject)  {
if ( subject->text_length + addNum + 1 >= subject->text_allocated_length ) {
size_t newSize = 1.5 * subject->text_allocated_length + addNum + 1;
char *tempStr = (char *) realloc(subject->text, newSize);
if ( tempStr == NULL ) {
throw_exception(NULL_POINTER_EXCEPTION, -1,
"Unable to allocate memory for string while doing meminspection!\n");
}
subject->text = tempStr;
subject->text_allocated_length = newSize;
}
return;
}
 void throw_exception(exception e, int lineNum, char *message, ...) {
va_list args;
va_start(args, message);
char cMessage[AVG_STRING_SIZE];
if ( lineNum == -1 ) {
strncpy(cMessage, "Internal Error [", AVG_STRING_SIZE);
vsnprintf(cMessage, AVG_STRING_SIZE, message, args);
strcat(cMessage, "]"); 
} else {
snprintf(cMessage, AVG_STRING_SIZE, "Line #%d", lineNum);
}
va_end(args);
switch (e) {
case ERRNO_EXCEPTION:
perror(cMessage);
break;
case NULL_POINTER_EXCEPTION:
fprintf(stderr, "%s: %s\n", cMessage, message);
break;
case INDEX_OUT_OF_BOUNDS_EXCEPTION:
fprintf(stderr, "%s: %s\n", cMessage, message);
break;
default:
break;
}
return;
}
