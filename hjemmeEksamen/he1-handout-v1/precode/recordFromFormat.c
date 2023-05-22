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
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int validCourse ( char* start, char* stop, char* course) {
    char *courseStart = strstr(start, course);
    if (courseStart < stop && courseStart > start) {
        return 1;
    }
    return 0;
}

Record* XMLtoRecord( char* buffer, int bufSize, int* bytesread ) {
    printf("HEISANN HOPP JEG SKAL LAGE DIN RECORD IDAG!!!\n%s\n", buffer);
    struct Record *rec = newRecord();
    const char *xmlStr = buffer;
    char *strStart;
    char *strEnd;  

    char *recordStart = strstr(xmlStr, "<record>");
    char *recordEnd = strstr(xmlStr, "</record>");
    int totSize = recordEnd + strlen("</record>") - recordStart;
    if (!(recordStart && recordEnd)) {
        //Invalid record
        return NULL;
    }
    
    strStart = strstr(xmlStr, "source=\"");
    if (strStart) {
        char src;
        memcpy(&src, strStart + strlen("source=\""), sizeof(char));
        printf("Source: %c\n", src);
        setSource(rec, src);
    }
    strStart = strstr(xmlStr, "dest=\"");
    if (strStart) {        
        char dst;
        memcpy(&dst, strStart + strlen("dest=\""), sizeof(char));
        printf("Dest: %c\n", dst);
        if (!dst) {return NULL;} // No destination = invalid record
        setDest(rec, dst);
    }
    strStart = strstr(xmlStr, "username=\"");
    if (strStart) {        
        strEnd = strstr(strStart, "\" />");
        int len = strlen(strStart) - strlen(strEnd) - strlen("username=\"");
        char buf[len];
        buf[len] = '\0';
        memcpy(&buf, strEnd-len, len); 
        printf("Name: %s\n", buf);
        setUsername(rec, buf);
    }
    strStart = strstr(xmlStr, "id=\"");
    if (strStart) {        
        strEnd = strstr(strStart, "\" />");
        int len = strlen(strStart) - strlen(strEnd) - strlen("id=\"");
        char buf[len];
        buf[len] = '\0';
        memcpy(&buf, strEnd-len, len); 
        printf("id: %d\n", *buf);
        setId(rec, (uint32_t) atoi(buf));
    }
    strStart = strstr(xmlStr, "group=\"");
    if (strStart) {        
        strEnd = strstr(strStart, "\" />");
        int len = strlen(strStart) - strlen(strEnd) - strlen("group=\"");
        char buf[len];
        buf[len] = '\0';
        memcpy(&buf, strEnd-len, len); 
        printf("group: %d\n", *buf);
        setGroup(rec, (uint32_t) atoi(buf));
    }
    strStart = strstr(xmlStr, "semester=\"");
    if (strStart) {        
        strEnd = strstr(strStart, "\" />");
        int len = strlen(strStart) - strlen(strEnd) - strlen("semester=\"");
        char buf[len];
        buf[len] = '\0';
        memcpy(&buf, strEnd-len, len); 
        printf("semester: %d\n", *buf);
        setSemester(rec, (uint8_t) atoi(buf));
    }
    strStart = strstr(xmlStr, "grade=\"");
    if (strStart) {
        strEnd = strstr(strStart, "\" />");
        int size = strlen(strStart) - strlen(strEnd) - strlen("grade=\"");
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
                break;
        }
        printf("grade: %d\n", size);
    }
    strStart = strstr(xmlStr, "<courses>");
    strEnd = strstr(xmlStr, "</courses>");
    if (strStart < recordStart || strEnd > recordEnd) { strStart = NULL; strEnd = NULL; }
    if (strStart && strEnd) {
        if (validCourse(strStart, strEnd, "IN1000")){ setCourse(rec, Course_IN1000); }
        if (validCourse(strStart, strEnd, "IN1010")){ setCourse(rec, Course_IN1010); }        
        if (validCourse(strStart, strEnd, "IN1020")){ setCourse(rec, Course_IN1020); }        
        if (validCourse(strStart, strEnd, "IN1030")){ setCourse(rec, Course_IN1030); }        
        if (validCourse(strStart, strEnd, "IN1050")){ setCourse(rec, Course_IN1050); }        
        if (validCourse(strStart, strEnd, "IN1060")){ setCourse(rec, Course_IN1060); }        
        if (validCourse(strStart, strEnd, "IN1080")){ setCourse(rec, Course_IN1080); }        
        if (validCourse(strStart, strEnd, "IN1140")){ setCourse(rec, Course_IN1140); }        
        if (validCourse(strStart, strEnd, "IN1150")){ setCourse(rec, Course_IN1150); }        
        if (validCourse(strStart, strEnd, "IN1900")){ setCourse(rec, Course_IN1900); }        
        if (validCourse(strStart, strEnd, "IN1910")){ setCourse(rec, Course_IN1910); }        
        printf("course: %d\n", rec->courses);
    }
    *bytesread = totSize;
    
    printf("TAKK FOR AT DU VENTEt, RECORDEN DIN ER FERDIG!!\n");
    return rec;
}


Record* BinaryToRecord( char* buffer, int bufSize, int* bytesread ) {
    struct Record *record = newRecord(); 
    printf("HEISANN HOPP JEG SKAL LAGE DIN RECORD IDAG!!!\n%s\n", buffer);
    
    char flag;
    memcpy(&flag, buffer, 1);
    buffer += sizeof(flag);
    *bytesread +=  sizeof(flag);
    
    if (!(flag & FLAG_DST)) { return NULL; } // No destination, invalid record

    if (flag & FLAG_SRC) {
        char src;
        memcpy(&src, buffer, 1);
        buffer += sizeof(char);
        *bytesread += sizeof(char);
        printf("Source: %c\n", src);
        setSource(record, src);
    }
    if (flag & FLAG_DST) {
        char dst;
        memcpy(&dst, buffer, 1);
        buffer += sizeof(char);
        *bytesread += sizeof(char);
        printf("Dest: %c\n", dst);
        setDest(record, dst);
    }
    if (flag & FLAG_USERNAME){
        uint32_t nameLen;
        memcpy(&nameLen, buffer, sizeof(uint32_t));
        nameLen = htonl(nameLen);
        buffer += sizeof(uint32_t);
       *bytesread += sizeof(uint32_t);
        printf("Namesize: %d\n", nameLen);

        char *name = malloc(nameLen);
        name[nameLen] = '\0';
        memcpy(name, buffer, nameLen);
        buffer += nameLen;
        *bytesread += nameLen;
        printf("Name: %s\n", name);
        setUsername(record, name);
        free(name);
    }
    if (flag & FLAG_ID) {
        uint32_t id;
        memcpy(&id, buffer, sizeof(uint32_t));
        id = ntohl(id);
        *bytesread += sizeof(uint32_t);
        buffer += sizeof(uint32_t);
        printf("id: %d\n", id);
        setId(record, id);
    }
    if (flag & FLAG_GROUP) {
        uint32_t group;
        memcpy(&group, buffer, sizeof(uint32_t));
        group = ntohl(group);
        buffer += sizeof(uint32_t);
        *bytesread += sizeof(uint32_t);
        printf("group: %d\n", group);
        setGroup(record, group);
    }
    if (flag & FLAG_SEMESTER) {
        uint8_t semester;
        memcpy(&semester, buffer, sizeof(uint8_t));
        buffer += sizeof(uint8_t);
        *bytesread += sizeof(uint8_t);
        printf("semester: %d\n", semester);
        setSemester(record, semester);
    }
    if (flag & FLAG_GRADE) {
        Grade grade;
        memcpy(&grade, buffer, sizeof(char));
        buffer += sizeof(char);
        *bytesread += sizeof(char); 
        printf("grade: %d\n", grade);
        setGrade(record, grade);
    } 
    if (flag & FLAG_COURSES) {
        uint16_t courses;
        memcpy(&courses, buffer, sizeof(uint16_t));
        courses = ntohs(courses);
        buffer += sizeof(uint16_t);
        *bytesread += sizeof(uint16_t);
        printf("courses: %d\n", courses);
        setCourse(record, courses);
    }

    printf("TAKK FOR AT DU VENTET, RECORDEN DIN ER FERDIG!!\n");
    return record;
}

