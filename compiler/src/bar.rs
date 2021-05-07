use indicatif::{ProgressBar, ProgressStyle};

/// Progress bar used to show numeric values, usually related to entries in dictionaries
pub fn progress_bar(len: usize, msg: &'static str) -> ProgressBar {
    let pbar = ProgressBar::new(len as u64).with_style(
        ProgressStyle::default_bar()
            .progress_chars("=> ")
            .template("{msg} [{wide_bar}] ETA: {eta} ({pos}/{len})"),
    );
    pbar.set_message(msg);
    pbar
}
