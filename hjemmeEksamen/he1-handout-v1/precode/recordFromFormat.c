/*
 * This file implements two functions that read XML and binary information from a buffer,
 * respectively, and return pointers to Record or NULL.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "recordFromFormat.h"
#include "record.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

Record* XMLtoRecord( char* buffer, int bufSize, int* bytesread ) {
    struct Record *rec = newRecord();
    const char *xmlStr = buffer;
    char *strStart;
    char *strEnd;  
    char *record = strstr(xmlStr, "</record>");
    if (!record) {
        fprintf(stderr, "Incomplete record: No closing tag");
        return NULL;
        }
    
    strStart = strstr(xmlStr, "source=\"");
    if (strStart) {
        char source;
        strEnd = strstr(xmlStr, "source \" />");
        int size = strEnd - strStart - strlen("source=\"");
        memcpy(&source, (void *)(strStart+size), size); //TODO This might not work
        setSource(rec, source);
    }
    strStart = strstr(xmlStr, "dest=\"");
    if (strStart) {        
        char dest;
        strEnd = strstr(xmlStr, "dest \" />");
        int size = strEnd - strStart - strlen("dest=\"");
        memcpy(&dest, (void *)(strStart+size), size); //TODO This might not work
        setDest(rec, dest);
    }
    strStart = strstr(xmlStr, "username=\"");
    if (strStart) {        
        char *username;
        strEnd = strstr(xmlStr, "username \" />");
        int size = strEnd - strStart - strlen("username=\"");
        memcpy(&username, (void *)(strStart+size), size); //TODO This might not work
        setUsername(rec, username);
    }
    strStart = strstr(xmlStr, "id=\"");
    if (strStart) {        
        char *id;
        strEnd = strstr(xmlStr, "id \" />");
        int size = strEnd - strStart - strlen("id=\"");
        memcpy(&id, (void *)(strStart+size), size); //TODO This might not work
        setId(rec, (uint32_t) atoi(id));
    }
    strStart = strstr(xmlStr, "group=\"");
    if (strStart) {        
        char *group;
        strEnd = strstr(xmlStr, "group \" />");
        int size = strEnd - strStart - strlen("group=\"");
        memcpy(&group, (void *)(strStart+size), size); //TODO This might not work
        setGroup(rec, (uint32_t) atoi(group));
    }
    strStart = strstr(xmlStr, "semester=\"");
    if (strStart) {        
        char *semester;
        strEnd = strstr(xmlStr, "semester \" />");
        int size = strEnd - strStart - strlen("semester=\"");
        memcpy(&semester, (void *)(strStart+size), size); //TODO This might not work
        setSemester(rec, (uint8_t) atoi(semester));
    }
    strStart = strstr(xmlStr, "grade=\"");
    if (strStart) {
        // char *grade;
        strEnd = strstr(xmlStr, "grade \" />");
        int size = strEnd - strStart - strlen("grade=\"");
        // memcpy(&grade, (void *)(strStart+size), size); //TODO This might not work
        // switch (strlen(grade)) 
        switch (size) {
            case 4:
                setGrade(rec, Grade_None);
                break;
            case 8:
                setGrade(rec, Grade_Bachelor);
                break;
            case 6:
                setGrade(rec, Grade_Master);
                break;
            case 3:
                setGrade(rec, Grade_PhD);
                break;
            default:
                setGrade(rec, Grade_None); //If no match, assumes no grade
        }
    }
    return rec;
}


Record* BinaryToRecord( char* buffer, int bufSize, int* bytesread ) {
    struct Record *record = newRecord(); 
    
    char flag;
    memcpy(&flag, buffer, 1);
    buffer += sizeof(flag);
    *bytesread +=  sizeof(flag);
    
    if (flag & FLAG_SRC) {
        char src;
        memcpy(&src, buffer, 1);
        buffer += sizeof(char);
        *bytesread += sizeof(char);
        setSource(record, src);
    }
    if (flag & FLAG_DST) {
        char dst;
        memcpy(&dst, buffer, 1);
        buffer += sizeof(char);
        *bytesread += sizeof(char);
        setDest(record, dst);
    }
    if (flag & FLAG_USERNAME){
        uint32_t nameLen;
        memcpy(&nameLen, buffer, sizeof(uint32_t));
        buffer += sizeof(uint32_t);
        *bytesread += sizeof(uint32_t);
        char *name[nameLen];
        memcpy(name, buffer, nameLen);
        buffer += nameLen;
        *bytesread += nameLen;
        setUsername(record, *name);
    }
    if (flag & FLAG_ID) {
        uint32_t id;
        memcpy(&id, buffer, sizeof(uint32_t));
        *bytesread += sizeof(uint32_t);
        buffer += sizeof(uint32_t);
        setId(record, id);
    }
    if (flag & FLAG_GROUP) {
        uint32_t group;
        memcpy(&group, buffer, sizeof(uint32_t));
        buffer += sizeof(uint32_t);
        *bytesread += sizeof(uint32_t);
    }
    if (flag & FLAG_SEMESTER) {
        uint8_t semester;
        memcpy(&semester, buffer, sizeof(uint8_t));
        buffer += sizeof(uint8_t);
        *bytesread += sizeof(uint8_t);
        setSemester(record, semester);
    }
    if (flag & FLAG_GRADE) {
        Grade grade;
        memcpy(&grade, buffer, sizeof(Grade));
        buffer += sizeof(Grade);
        *bytesread += sizeof(Grade); 
        setGrade(record, grade);
    } 
    if (flag & FLAG_COURSES) {
        uint16_t courses;
        memcpy(&courses, buffer, sizeof(uint16_t));
        buffer += sizeof(uint16_t);
        *bytesread += sizeof(uint16_t);
        setCourse(record, courses);
    }

    return record;
}

