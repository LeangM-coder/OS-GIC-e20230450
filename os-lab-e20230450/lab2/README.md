# OS Lab 2: Linux Navigation & File Management

## What I Did

I put two reports (`budget 2026.txt` and `audit notes.txt`) into the right TechCorp folders without changing them. My departments were Finance (owner), HR (reader) and Support (third).

- **Core 1:** Made the folders with one `mkdir -p` and checked them with `find TechCorp -type d`.
- **Core 2:** Moved both files to `reports/` (one with a relative path, one with absolute paths), copied them to `TechCorp/Finance/`, and made `first-original.txt` in `archive/`. I also read the Finance file from inside `HR` with a relative and an absolute path.
- **Core 3:** Used `cmp` to check the copies. It printed nothing, so they are identical. Then I saved the file list with `find ... | sort | tee evidence/tree.txt`.

## My Path Error

From inside `TechCorp/HR` I ran `cat "reports/budget 2026.txt"` and got:

```
cat: 'reports/budget 2026.txt': No such file or directory
```

The file exists, but there is no `reports` folder inside `HR`. A relative path starts from where I am now. The fix was `../../reports/budget 2026.txt` (two `..` to get back to the lab folder).

## Relative or Absolute?

For step 3 I used relative paths because I was already in the lab folder and it was shorter. Absolute paths are better when I am deep in another folder, because they work from anywhere.

## What Was Hard

- File names with spaces need double quotes, or the shell splits them into two names.
- Counting `..` was confusing, so I ran `pwd` a lot.

## What I Learned

- Run `pwd` before `cp` or `mv`.
- `..` always counts from the folder I am in.
- `cmp` is the real proof that a copy is the same.
