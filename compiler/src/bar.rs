use indicatif::{ProgressBar, ProgressStyle};

/// Progress bar used to show numeric values, usually related to entries in dictionaries
pub fn progress_bar(len: usize, msg: &str) -> ProgressBar {
    let pbar = ProgressBar::new(len as u64).with_style(
        ProgressStyle::default_bar()
            .progress_chars("=> ")
            .template("{msg} [{wide_bar}] ETA: {eta} ({pos}/{len})"),
    );
    pbar.set_message(msg);
    pbar
}

/// Progress bar used to show size values in bytes
pub fn progress_bar_bytes(len: usize, msg: &str) -> ProgressBar {
    let pbar = ProgressBar::new(len as u64).with_style(
        ProgressStyle::default_bar()
            .progress_chars("=> ")
            .template("{msg} [{wide_bar}] ETA: {eta} ({bytes}/{total_bytes} {bytes_per_sec})"),
    );
    pbar.set_message(msg);
    pbar
}
