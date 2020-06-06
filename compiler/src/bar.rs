use indicatif::{ProgressBar, ProgressStyle};

pub fn progress_bar(len: usize, msg: &str) -> ProgressBar {
    let bar = ProgressBar::new(len as u64)
        .with_style(ProgressStyle::default_bar()
            .progress_chars("=> ")
            .template("{msg} [{wide_bar}] ETA: {eta} ({pos}/{len})"));
    bar.set_message(msg);
    bar
}

pub fn progress_bar_bytes(len: usize, msg: &str) -> ProgressBar {
    let bar = ProgressBar::new(len as u64)
        .with_style(ProgressStyle::default_bar()
            .progress_chars("=> ")
            .template("{msg} [{wide_bar}] ETA: {eta} ({bytes}/{total_bytes} {bytes_per_sec})"));
    bar.set_message(msg);
    bar
}
