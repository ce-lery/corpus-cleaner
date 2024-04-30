# Contribution Guide

A guide on how to contribute to this repository.  

## Contents

- [Contents](#contents)
- [Directory structure](#directory-structure)
- [Development environment](#development-environment)
- [Run](#run)
- [Branch flow](#branch-flow)
- [Issues](#issues)
- [Pull Request](#pull-request)
- [How to send corrections](#how-to-send-corrections)
- [Review](#review)
- [Commit message convention](#commit-message-convention)
- [Coding conventions](#coding-conventions)
- [Release flow](#release-flow)
- [Reference](#reference)

## Directory structure

Place the required files in each of the following locations.

```
ThisRepository
  |-- docs              : Document folder for specifications, manuals, etc.
  |-- corpus_cleaner    : Source code
  |-- scripts           : shell scripts
  |-- tests             : Test code
  |-- .gitignore        : Non-commit settings
  |-- CONTRIBUTING.md   : This file
  `-- README.md         : Overview of this repository
```

## Development environment

### Version information

The various versions are as follows.

|item|verion|remarks|  
|:-- |:-- |:--|
|OS|Ubuntu 22.04.3 LTS||

### Environment construction procedure

Plese refer [README](README.md).

## Run

### Run procedure

Plese refer [README](README.md).

## Branch flow

Plese use [GitHub Flow](https://docs.github.com/ja/get-started/quickstart/github-flow).  
This repository uses the following two types of branches.

|branch name|purpose|Delete this branch?|How to merge into this branch (Pull Request)|
|:-- |:-- |:--|:--|
|main|Branch under development. When the release version is completed, tag it and release it.|No|Merge Commit|
|xx_xx_xx|Cut a branch for each function (for each developer) from main and create this branch. <br>The branch name should represent the implementation content, such as "increase-test-timeout" or "add-code-of-conduct". <br>Once implementation is complete, merge into main|Yes<br>Removal is recommended|-<br> (**Prohibits branching from this branch and prohibits issuing pull requests to this branch.** Cut from main branch.)|

## Issues 

I'm accepting the following issues. Please post from the issues page.

- Questions about documents and contents
- Report content errors or problems
- Suggest improvements to explanations
- Suggest new topics etc.

Other issues are also welcome.

## Pull Request

Pull requests are always welcome.

I accept the following types of pull requests: For basic pull requests (especially detailed ones), there is no problem if you send a pull request without creating an issue.

- Fixed typos
- Sample code and spelling corrections
- Suggest or modify another explanation method
- Improved the text to make it easier to understand
- Website improvements
- Test improvements

If you have any questions, such as "What about fixes/improvements like this?", please raise an issue and discuss.

## How to send corrections

How you send your fixes depends on whether you are a co-developer or not.
You are a Collaborators if your name is set as Admin or Developer in the Collaborators section of this repository's settings page.

### Collaborator

If you are a co-developer, you can make changes directly to this repository.  
Please follow the steps below.

1. Clone this repository
2. Checkout to main branch: git checkout main
3. Create a feature branch: git checkout -b [arbitrary_branch_name]
4. Check that unittest passes
5. Commit your changes: git commit -am 'feat: [changes]'
6. Push: git push origin [arbitrary_branch_name]
7. Send a pull request

### Not Collaborator

Please follow the steps below.

1. Fork and clone the Fork repository
2. Checkout to main branch: git checkout main
3. Create a branch: git checkout -b [arbitrary_branch_name]
4. Check that unittest passes
5. Commit your changes: git commit -am 'feat: [changes]'
6. Push: git push origin [arbitrary_branch_name]
7. Send a pull request

## Review

### What to do before the review

- We will conduct the test according to the test method.

### What to do after the review

- Merge with Rebase.
- Delete working branch.

## Commit message convention

Based on [Angular's Git Commit Guidelines](https://github.com/conventional-changelog/conventional-changelog/blob/master/packages/conventional-changelog-angular/README.md).

In the form below

- Summary on the first line
- 2nd line is blank
- Main text from line 3

Finally, write a related issue (optional).
By writing something like `fix #<issue number>`, you can automatically close the issue when merging the PR.

- [Linking a pull request to an issue - GitHub Docs](https://docs.github.com/en/issues/tracking-your-work-with-issues/linking-a-pull-request-to-an-issue)

```
feat(ngInclude): add template url parameter to events

The `src` (i.e. the url of the template to load) is now provided to the
`$includeContentRequested`, `$includeContentLoaded` and `$includeContentError`
events.

Closes #8453
Closes #8454
```


```
                         scope        commit title

        commit type       /                /      
                \        |                |
                 feat(ngInclude): add template url parameter to events

        body ->  The 'src` (i.e. the url of the template to load) is now provided to the
                 `$includeContentRequested`, `$includeContentLoaded` and `$includeContentError`
                 events.

 referenced  ->  Closes #8453
 issues          Closes #8454
```


The `commit types` include:

- feat  
  - Add new features, chapters, sections, etc.  
  - Add new pages to be included in update history  
- fix  
  - Corrections that change the meaning  
  - Fixes that appear in the update history  
- docs  
    <!-- - Basically not used --> 
  - About README.md, CONTRIBUTING.md, and the main project's entire document  
- refactor  
  - Corrections that do not change the meaning  
  - Fixes that do not appear in the update history  
- style  
  - Adjust spacing and indentation  
  - Fixed Lint errors, etc.  
- perf  
  - Performance improvements  
- test  
  - About test  
- chore  
  - others  
  - Fixed typos etc.  


## Coding conventions

The markdown rules are asically [markdownlint rule](https://marketplace.visualstudio.com/items?itemName=DavidAnson.vscode-markdownlint#:~:text=language%20identifier.-,Rules,-MD001%20heading%2Dincrement).  
Please use [markdownlint extension](https://marketplace.visualstudio.com/items?itemName=DavidAnson.vscode-markdownlint).  

## Release flow

The steps are as follows:

1. Merge from develop branch to main branch
2. Tag commits in the main branch with a version number
3. Release with Releases

The version number will be assigned in accordance with [Semantic Versioning ver.2.0.0](https://semver.org/lang/ja/).

## Reference

- [puppetlabs/puppet](https://github.com/puppetlabs/puppet/blob/8.3.1/CONTRIBUTING.md)
- [asciidwango/js-primer](https://github.com/asciidwango/js-primer/blob/master/CONTRIBUTING.md)
<!-- - [I created guidelines for creating issues and PRs that I had been doing within the team, and transcribed them into CONTRIBUTING.md.](https://dev.classmethod.jp/articles/output-contributing-md-for-team-building/) -->