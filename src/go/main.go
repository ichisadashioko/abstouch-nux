package main

import (
  "fmt"
  "os"
  "strings"
)

func main() {
  args := os.Args[1:]
  var options, other_args []string

  for i := 0; i < len(args); i++ {
    if strings.HasPrefix(args[i], "-") {
      options = append(options, args[i])
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

  command := strings.ToLower(other_args[0])
  if command == "help" {
    fmt.Println("\033[1;32m---====\033[1;37mabstouch-nux\033[1;32m====---")
    fmt.Println("\033[1;32m => \033[1;37mUsage: \033[;mabstouch <command> [options]")
    fmt.Println("")
    fmt.Println("\033[1;32m---======\033[1;37mCommands\033[1;32m======---")
    fmt.Println("\033[1;32m => \033[;mhelp \033[1;32m=> \033[;mShows this text.")
    fmt.Println("\033[1;32m => \033[;mstart \033[1;32m=> \033[;mStarts abstouch-nux.")
    fmt.Println("\033[1;32m => \033[;mstop \033[1;32m=> \033[;mTerminates abstouch-nux.")
    fmt.Println("")
    fmt.Println("\033[1;32m---====================---")
    fmt.Println("\033[1;32m => \033[;mAlso see \033[1;37mabstouch.1 \033[;mfor examples and more.")
    os.Exit(0)
  }

  if command == "start" {
    fmt.Println("\033[1;31m => Not implemented yet!")
    os.Exit(1)
  }

  if command == "stop" {
    fmt.Println("\033[1;31m => Not implemented yet!")
    os.Exit(1)
  }

  fmt.Printf("\033[1;31m => \033[1;37mUnknown command: \033[;m%s\n", command)
  fmt.Println("\033[1;32m => \033[1;37mUsage: \033[;mabstouch <command> [options]")
  fmt.Println("\033[1;32m => \033[1;37mSee: \033[;mabstouch help")
  os.Exit(1)
}
