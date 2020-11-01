package main

import (
  "fmt"
  "os"
  "os/exec"
  "strings"
)

func Log(s string, b bool) {
  if b {
    fmt.Printf(s)
  }
}

func StartDaemon(args ...string) (p *os.Process, err error) {
  if args[0], err = exec.LookPath(args[0]); err == nil {
    var procAttr os.ProcAttr
    procAttr.Files = []*os.File{os.Stdin,
      os.Stdout, os.Stderr}
    p, err := os.StartProcess(args[0], args, &procAttr)
    if err == nil {
      return p, nil
    }
  }
  return nil, err
}

func Start(args ...string) (err error) {
  if args[0], err = exec.LookPath(args[0]); err == nil {
    cmd := exec.Command(args[0])
    cmd.Stdout = os.Stdout
    cmd.Stderr = os.Stderr
    cmd.Run()
    return nil
  }
  return err
}

func main() {
  args := os.Args[1:]

  start_command := "/usr/share/abstouch-nux/input"

  var option_args, other_args []string
  for i := 0; i < len(args); i++ {
    if strings.HasPrefix(args[i], "-") {
      option_args = append(option_args, args[i])
    } else {
      other_args = append(other_args, args[i])
    }
  }

  if len(other_args) < 1 {
    fmt.Println("\033[1;31m => \033[1;37mPlease provide a command!")
    fmt.Println("\033[1;32m => \033[1;37mUsage: \033[;mabstouch <command> [options]")
    fmt.Println("\033[1;32m => \033[1;37mSee: \033[;mabstouch help")
    os.Exit(1)
  }

  var options []string
  if len(option_args) > 0 {
    for i := 0; i < len(option_args); i++ {
      if strings.HasPrefix(option_args[i], "--") {
        options = append(options, strings.Replace(option_args[i], "--", "", 1))
      } else {
        opt_str := strings.Replace(option_args[i], "-", "", 1)
        for v := 0; v < len(opt_str); v++ {
          options = append(options, string(opt_str[v]))
        }
      }
    }
  }

  var quiet, daemon bool
  for i := 0; i < len(options); i++ {
    if options[i] == "q" || options[i] == "quiet" {
      quiet = true
    } else if options[i] == "d" || options[i] == "daemon" {
      daemon = true
    }
  }

  command := strings.ToLower(other_args[0])
  if command == "help" {
    fmt.Println("\033[1;32m---====\033[1;37mabstouch-nux\033[1;32m====---")
    fmt.Println("\033[1;32m => \033[1;37mUsage: \033[;mabstouch <command> [options]")
    fmt.Println("")
    fmt.Println("\033[1;32m---======\033[1;37mCommands\033[1;32m======---")
    fmt.Println("\033[1;32m => \033[;mhelp \033[1;32m=> \033[;mShows this text.")
    fmt.Println("\033[1;32m => \033[;mstart \033[1;32m=> \033[;mStarts abstouch-nux.")
    fmt.Println("\033[1;32m => \033[;mstop \033[1;32m=> \033[;mTerminates abstouch-nux daemon.")
    fmt.Println("")
    fmt.Println("\033[1;32m---=======\033[1;37mOptions\033[1;32m======---")
    fmt.Println("\033[1;32m => \033[;m-q\033[1;32m, \033[;m--quiet \033[1;32m=> \033[;mDisables output except err.")
    fmt.Println("\033[1;32m => \033[;m-d\033[1;32m, \033[;m--daemon \033[1;32m=> \033[;mRuns in background.")
    fmt.Println("")
    fmt.Println("\033[1;32m---====================---")
    fmt.Println("\033[1;32m => \033[;mAlso see \033[1;37mabstouch.1 \033[;mfor examples and more.")
    os.Exit(0)
  }

  if command == "start" {
    if !daemon {
      Log("\033[1;32m => \033[;mStarting abstouch-nux...\n", !quiet)
      if err := Start(start_command); err != nil {
        fmt.Println("\033[1;31m => \033[;mStarting abstouch-nux... Failed!")
        fmt.Println(err)
        os.Exit(1)
      }
    } else {
      Log("\033[1;32m => \033[;mStarting abstouch-nux...\r", !quiet)
      if _, err := StartDaemon(start_command); err == nil {
        Log("\033[1;32m => \033[;mStarting abstouch-nux... Success!\n", !quiet)
      } else {
        fmt.Println("\033[1;31m => \033[;mStarting abstouch-nux... Failed!")
        fmt.Println(err)
        os.Exit(1)
      }
    }

    os.Exit(0)
  }

  if command == "stop" {
    fmt.Println("\033[1;31m => \033[1;37mNot implemented yet!\033[;m")
    os.Exit(1)
  }

  fmt.Printf("\033[1;31m => \033[1;37mUnknown command: \033[;m%s\n", command)
  fmt.Println("\033[1;32m => \033[1;37mUsage: \033[;mabstouch <command> [options]")
  fmt.Println("\033[1;32m => \033[1;37mSee: \033[;mabstouch help")
  os.Exit(1)
}
