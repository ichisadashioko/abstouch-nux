use std::io;
use std::env;
use std::panic;
use std::process;
use std::io::Write;

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
    panic::set_hook(Box::new(|_info| {}));

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
        println!("\x1b[1;32m => \x1b[;mhelp \x1b[1;32m=> \x1b[;mShows this text.");
        println!("\x1b[1;32m => \x1b[;mstart \x1b[1;32m=> \x1b[;mStarts abstouch-nux.");
        println!("\x1b[1;32m => \x1b[;mstop \x1b[1;32m=> \x1b[;mTerminates abstouch-nux daemon.");
        println!("");
        println!("\x1b[1;32m---=======\x1b[1;37mOptions\x1b[1;32m======---");
        println!("\x1b[1;32m => \x1b[;m-q\x1b[1;32m, \x1b[;m--quiet \x1b[1;32m=> \x1b[;mDisables output except err.");
        println!("\x1b[1;32m => \x1b[;m-d\x1b[1;32m, \x1b[;m--daemon \x1b[1;32m=> \x1b[;mRuns in background.");
        println!("");
        println!("\x1b[1;32m---====================---");
        println!("\x1b[1;32m => \x1b[;mAlso see \x1b[1;37mabstouch.1 \x1b[;mfor examples and more.");
        process::exit(0);
    }
    else if command == "start"
    {
        let mut args = vec![];
        if !daemon
        {
            args.push("-v");
        }

        log(String::from("\x1b[1;32m => \x1b[;mStarting abstouch-nux...\r"), quiet);
        let result = panic::catch_unwind(|| {
            let mut command = process::Command::new("/usr/share/abstouch-nux/input")
                .args(args)
                .spawn()
                .expect("\x1b[1;31m => \x1b[;mStarting abstouch-nux... Failed!");
            if daemon
            {
                log(String::from("\x1b[1;32m => \x1b[;mStarting abstouch-nux... Success!\n"), quiet);
            }
            else
            {
                log(String::from("\n"), quiet);
                command.wait()
                    .expect("\x1b[1;31m => \x1b[;mStarting abstouch-nux... Failed!");
            }
        });

        match result {
            Ok(res) => res,
            Err(_) => println!("\x1b[1;31m => \x1b[;mStarting abstouch-nux... Failed!")
        };
    }
    else if command == "stop"
    {
        println!("\x1b[1;31m => \x1b[1;37mNot implemented yet!\x1b[;m");
        process::exit(1);
    }
    else
    {
        println!("\x1b[1;31m => \x1b[1;37mUnknown command: \x1b[;m{}\n", command);
        println!("\x1b[1;32m => \x1b[1;37mUsage: \x1b[;mabstouch <command> [options]");
        println!("\x1b[1;32m => \x1b[1;37mSee: \x1b[;mabstouch help");
        process::exit(1);
    }
}
