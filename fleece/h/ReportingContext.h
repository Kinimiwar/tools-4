#ifndef _REPORTING_CONTEXT_H_
#define _REPORTING_CONTEXT_H_

#include <fcntl.h>
#include <list>
#include <map>
#include <queue>
#include <vector>
#include "Architecture.h"
#include "Assembly.h"
#include "FieldList.h"
#include "Reassemble.h"
#include "Report.h"
#include "StringUtils.h"

#define REPORT_FILENAME_BUF_LEN 512

/*
 * This class is used to keep track of the instruction templates we have
 * already seen. It filters reports that are similar to the ones we have
 * already seen and attempts to produce new ones.
 */
class ReportingContext {

public:

    /*
     * Creates a reporting context with the given output file. The file will
     * be flushed every flushFreq reports.
     */
    ReportingContext(const char* outputDir, int flushFreq);

    /*
     * Destroys the reporting context. Does NOT close the output file (the
     * reporting context did not open it).
     */
    ~ReportingContext();

    /*
     * Adds the name of a decoder to the list of decoders that this reporting
     * context is receiving instructions from.
     */
    void addDecoder(const char* name);

    /*
     * Takes an array of decoded instructions, and the bytes and produces a
     * report, if one should be produced based on what was previously seen.
     */
    int processDecodings(std::vector<Assembly*>& asmList);

    /*
     * Prints data about the activity of the reporting context.
     */
    void printSummary(FILE* outf);

    /*
     * Accessors for numerical data about reporting.
     */
    unsigned int getNumReports() { return nReports; }
    unsigned int getNumMatches() { return nMatches; }
    unsigned int getNumProcessed() { return nProcessed; }
    unsigned int getNumSuppressed() { return nSuppressed; }

private:

    /*
     * Writes a header containing the list of decoders to the file outf.
     */
    void writeHeader(FILE* outf);
   
    /*
     * Reports a difference to the file that was passed at creation time.
     */
    void reportDiff(Report* report);

    /*
     * Examines the data already reported and decides if the incoming decodings
     * need to be reported as well.
     */
    bool shouldReportDiff(Report* report);

    /*
     * Writes all waiting reports out to files.
     */
    void flushReportQueue();

    /*
     * Closes all files currently open in this reporting context.
     */
    void closeOpenFiles();
    
    /*
     * Returns an open file by name. If this reporting context already
     * has this file open, it will 
     */
    FILE* getOpenFileByName(const char* filename);

    /*
     * Data used to summarize the activity of the reporting context.
     */
    unsigned int nReports;
    unsigned int nMatches;
    unsigned int nProcessed;
    unsigned int nSuppressed;

    /*
     * The record of different instruction decodings seen.
     */
    std::map<char*, std::list<Report*>*, StringUtils::str_cmp>* diffMap;
    std::map<char*, Report*, StringUtils::str_cmp>* matchMap;
    std::map<char*, FILE*, StringUtils::str_cmp>* fileMap;

    /*
     * The output dir for all reports (but not necessarily for summary data).
     */
    char* outputDir;

    /*
     * The frequency (in reports) with which the report file should be flushed.
     */
    int flushFreq;

    /*
     * The number of reports issued since the last flush.
     */
    int flushCount;

    /*
     * A list of the names of each decoder. When processDecodings is called, it
     * will be assumed that the ith instruction is from the ith decoder in this
     * list.
     */
     std::vector<char*> decoderNames;

     /*
      * A queue of all reports waiting to be written to output files.
      */
     std::queue<Report*> reportQueue;
};

#endif // _REPORTING_CONTEXT_H_
