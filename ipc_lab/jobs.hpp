#ifndef __TSH_JOBS__
#define __TSH_JOBS__

#include "config.hpp"
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <vector>

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

struct Job {           /* The job struct */
  pid_t pid;           /* job PID */
  int jid;             /* job ID [1, 2, ...] */
  int state;           /* UNDEF, BG, FG, or ST */
  std::string cmdline; /* command line */

  Job(pid_t pid = 0, int jid = 0, int state = UNDEF, std::string cmdline = "")
      : pid(pid), jid(jid), state(state), cmdline(cmdline) {}
};
struct Job jobs[MAXJOBS]; /* The job list */

extern int verbose;

/* Job Class */

class JobManager {
private:
  std::vector<Job> jobs;
  int nextjid;

public:
  JobManager(unsigned max_jobs = MAXJOBS) : nextjid(1) {
    jobs.reserve(max_jobs);
  }

  /* maxjid - Returns largest allocated job ID */
  int maxjid() const {
    const unsigned job_cnt = jobs.size();

    int max = 0;
    for (unsigned i = 0; i < job_cnt; i++) {
      if (jobs[i].jid > max) {
        max = jobs[i].jid;
      }
    }
    return max;
  }

  /* addjob - Add a job to the job list */
  bool addjob(pid_t pid, int state, std::string cmdline) {
    if (pid < 1) {
      throw std::invalid_argument(__FUNCTION__);
    }

    jobs.emplace_back(pid, nextjid++, state, cmdline);
    if (verbose) {
      const Job &job = jobs.back();
      printf("Added job [%d] %d %s\n", job.jid, job.pid, job.cmdline.c_str());
    }

    return true;
  }

  /* deletejob - Delete a job whose PID=pid from the job list */
  bool deletejob(pid_t pid) {
    if (pid < 1) {
      throw std::invalid_argument(__FUNCTION__);
    }

    for (auto it = jobs.begin(); it != jobs.end(); it++) {
      if (it->pid == pid) {
        jobs.erase(it);
        return true;
      }
    }

    return false;
  }

  /* fgpid - Return PID of current foreground job, 0 if no such job */
  pid_t fgpid() const {
    const unsigned job_cnt = jobs.size();
    for (unsigned i = 0; i < job_cnt; i++) {
      if (jobs[i].state == FG) {
        return jobs[i].pid;
      }
    }
    return 0;
  }

  /* getjobpid  - Find a job (by PID) on the job list */
  Job *getjobpid(pid_t pid) {
    if (pid < 1) {
      throw std::invalid_argument(__FUNCTION__);
    }

    const unsigned job_cnt = jobs.size();
    for (unsigned i = 0; i < job_cnt; i++) {
      if (jobs[i].pid == pid) {
        return &jobs[i];
      }
    }

    return NULL;
  };

  Job *getjobjid(int jid) {
    if (jid < 1) {
      throw std::invalid_argument(__FUNCTION__);
    }

    const unsigned job_cnt = jobs.size();
    for (unsigned i = 0; i < job_cnt; i++) {
      if (jobs[i].jid == jid) {
        return &jobs[i];
      }
    }

    return NULL;
  };

  /* pid2jid - Map process ID to job ID */
  int pid2jid(pid_t pid) {
    if (pid < 1) {
      throw std::invalid_argument(__FUNCTION__);
    }

    const unsigned job_cnt = jobs.size();
    for (unsigned i = 0; i < job_cnt; i++) {
      if (jobs[i].pid == pid) {
        return jobs[i].jid;
      }
    }
    return 0;
  }

  /* listjobs - Print the job list */
  void listjobs() {
    const unsigned job_cnt = jobs.size();
    for (unsigned i = 0; i < job_cnt; i++) {
      if (jobs[i].pid != 0) {
        printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
        switch (jobs[i].state) {
        case BG:
          printf("Running ");
          break;
        case FG:
          printf("Foreground ");
          break;
        case ST:
          printf("Stopped ");
          break;
        default:
          printf("listjobs: Internal error: job[%d].state=%d ", i,
                 jobs[i].state);
        }
        printf("%s", jobs[i].cmdline.c_str());
      }
    }
  }
};
#endif