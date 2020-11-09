use std::io;
use std::fs;
use std::env;
use std::panic;
use std::process;
use std::io::Write;
use sysinfo::{Signal, System, SystemExt, ProcessExt};

fn log(s: String, b: bool)
{
    if !b
    {
        print!("{}", s);
        io::stdout().flush().unwrap();
    }
}

fn main()
{
    let s = System::new_all();
    panic::set_hook(Box::new(|_info| {}));

    let input_bin = vec!["abstouch-nux-input", "/usr/share/abstouch-nux/bin/abstouch-nux-input"];
    let set_event_bin = vec!["abstouch-nux-set_event", "/usr/share/abstouch-nux/bin/abstouch-nux-set_event"];
    let set_offset_bin = vec!["abstouch-nux-set_offset", "/usr/share/abstouch-nux/bin/abstouch-nux-set_offset"];
    let main_bin = input_bin;

    let mut args: Vec<String> = env::args().collect();
    let (mut option_args, mut other_args) = (vec![], vec![]);
    args.remove(0);

    for i in &args
    {
        if i.starts_with('-')
        {
            option_args.push(i.to_string());
        }
        else
        {
            other_args.push(i.to_string());
        }
    }

    if other_args.len() < 1
    {
        println!("\x1b[1;31m => \x1b[1;37mPlease provide a command!");
        println!("\x1b[1;32m => \x1b[1;37mUsage: \x1b[;mabstouch <command> [options]");
        println!("\x1b[1;32m => \x1b[1;37mSee: \x1b[;mabstouch help");
        process::exit(1);
    }

    let mut options = vec![];
    if option_args.len() > 0 {
        for i in &option_args
        {
            let mut opt = i.to_string();
            opt.remove(0);
            if i.starts_with("--")
            {
                opt.remove(0);
                options.push(opt);
            }
            else
            {
                for v in opt.chars()
                {
                    options.push(v.to_string());
                }
            }
        }
    }

    let (mut quiet, mut daemon) = (false, false);
    for i in &options
    {
        if i == "q" || i == "quiet"
        {
            quiet = true;
        }
        else if i == "d" || i == "daemon"
        {
            daemon = true;
        }
    }

    let command = other_args[0].to_lowercase();
    if command == "help"
    {
        println!("\x1b[1;32m---====\x1b[1;37mabstouch-nux\x1b[1;32m====---");
        println!("\x1b[1;32m => \x1b[1;37mUsage: \x1b[;mabstouch <command> [options]");
        println!("");
        println!("\x1b[1;32m---======\x1b[1;37mCommands\x1b[1;32m======---");
        println!("\x1b[1;32m => \x1b[;mhelp \x1b[1;32m=> \x1b[;mShows help text.");
        println!("\x1b[1;32m => \x1b[;mstart \x1b[1;32m=> \x1b[;mStarts abstouch-nux.");
        println!("\x1b[1;32m => \x1b[;mstop \x1b[1;32m=> \x1b[;mTerminates abstouch-nux daemon.");
        println!("\x1b[1;32m => \x1b[;msetevent \x1b[1;32m=> \x1b[;mSet event to match touchpad.");
        println!("\x1b[1;32m => \x1b[;msetoffset \x1b[1;32m=> \x1b[;mSet cursor offset to match your touchpad.");
        println!("");
        println!("\x1b[1;32m---=======\x1b[1;37mOptions\x1b[1;32m======---");
        println!("\x1b[1;32m => \x1b[;m-q\x1b[1;32m, \x1b[;m--quiet \x1b[1;32m=> \x1b[;mDisables output except err.");
        println!("\x1b[1;32m => \x1b[;m-d\x1b[1;32m, \x1b[;m--daemon \x1b[1;32m=> \x1b[;mRuns in background.");
        println!("");
        println!("\x1b[1;32m---====================---");
        println!("\x1b[1;32m => \x1b[;mAlso see \x1b[1;37mabstouch.1 \x1b[;mman page for examples and more.");
        process::exit(0);
    }
    else if command == "start"
    {
        let result = panic::catch_unwind(|| {
            let event_contents = fs::read_to_string("/usr/share/abstouch-nux/event.conf")
                .expect("\x1b[1;31m => Couldn't read event.conf!");
            let xoff_contents = fs::read_to_string("/usr/share/abstouch-nux/xoff.conf")
                .expect("\x1b[1;31m => Couldn't read xoff.conf!");
            let yoff_contents = fs::read_to_string("/usr/share/abstouch-nux/yoff.conf")
                .expect("\x1b[1;31m => Couldn't read yoff.conf!");
            if event_contents == "-1" || event_contents == "-1\n"
            {
                println!("\x1b[1;31m => \x1b[;mEvent not set!");
                println!("\x1b[1;32m => \x1b[;mTry using the following command:");
                println!("\x1b[1;32m => \x1b[;mabstouch setevent");
                process::exit(1);
            }
            else
            {
                let mut args = vec![];
                if !daemon && !quiet
                {
                    args.push("-v");
                }

                let eventarg = "-event".to_owned() + &event_contents[..];
                let xoffarg = "-xoff".to_owned() + &xoff_contents[..];
                let yoffarg = "-yoff".to_owned() + &yoff_contents[..];
                args.push(&eventarg[..]);
                args.push(&xoffarg[..]);
                args.push(&yoffarg[..]);

                if s.get_process_by_name(main_bin[0]).len() > 0
                {
                    println!("\x1b[1;31m => \x1b[;mThere is already an abstouch-nux daemon running!");
                    process::exit(1);
                }
                else
                {
                    log(String::from("\x1b[1;32m => \x1b[;mStarting abstouch-nux...\r"), quiet);

                    let mut proc = process::Command::new(main_bin[1])
                        .args(args)
                        .spawn()
                        .unwrap();
                    if daemon
                    {
                        log(String::from("\x1b[1;32m => \x1b[;mStarting abstouch-nux... Success.\n"), quiet);
                    }
                    else
                    {
                        log(String::from("\n"), quiet);
                        proc.wait()
                            .unwrap();
                    }
                }
            }
        });

        match result {
            Ok(res) => res,
            Err(_) => println!("\x1b[1;31m => \x1b[;mStarting abstouch-nux... Failed!")
        }
    }
    else if command == "stop"
    {
        let proc_arr = s.get_process_by_name(main_bin[0]);
        if proc_arr.len() > 0
        {
            for proc in proc_arr
            {
                proc.kill(Signal::Term);
            }
            println!("\x1b[1;32m => \x1b[;mSuccessfully terminated abstouch-nux daemon.");
            process::exit(0);
        }
        else
        {
            println!("\x1b[1;31m => \x1b[;mNo abstouch-nux daemon found!");
            process::exit(1);
        }
    }
    else if command == "setevent"
    {
        let result = panic::catch_unwind(|| {
            let mut proc = process::Command::new(set_event_bin[1])
                .spawn()
                .unwrap();
            proc.wait()
                .unwrap();
        });

        match result {
            Ok(res) => res,
            Err(_) => println!("\x1b[1;31m => \x1b[;mAn error occured!")
        }
    }
    else if command == "setoffset"
    {
        let result = panic::catch_unwind(|| {
            let mut proc = process::Command::new(set_offset_bin[1])
                .spawn()
                .unwrap();
            proc.wait()
                .unwrap();
        });

        match result {
            Ok(res) => res,
            Err(_) => println!("\x1b[1;31m => \x1b[;mAn error occured!")
        }
    }
    else
    {
        println!("\x1b[1;31m => \x1b[1;37mUnknown command: \x1b[;m{}\n", command);
        println!("\x1b[1;32m => \x1b[1;37mUsage: \x1b[;mabstouch <command> [options]");
        println!("\x1b[1;32m => \x1b[1;37mSee: \x1b[;mabstouch help");
        process::exit(1);
    }
}
