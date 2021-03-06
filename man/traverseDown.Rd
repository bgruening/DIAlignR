% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/merge_order.R
\name{traverseDown}
\alias{traverseDown}
\title{Traverses down from the root to leaves}
\usage{
traverseDown(
  tree,
  dataPath,
  fileInfo,
  multipeptide,
  prec2chromIndex,
  mzPntrs,
  precursors,
  adaptiveRTs,
  refRuns,
  params,
  applyFun = lapply
)
}
\arguments{
\item{tree}{(phylo) a phylogenetic tree.}

\item{dataPath}{(string) path to xics and osw directory.}

\item{fileInfo}{(data-frame) output of \code{\link{getRunNames}}.}

\item{multipeptide}{(environment) contains multiple data-frames that are collection of features
associated with analytes. This is an output of \code{\link{getMultipeptide}}.}

\item{prec2chromIndex}{(list) a list of dataframes having following columns: \cr
transition_group_id: it is PRECURSOR.ID from osw file. \cr
chromatogramIndex: index of chromatogram in mzML file.}

\item{mzPntrs}{(list) a list of mzRpwiz.}

\item{precursors}{(data-frame) atleast two columns transition_group_id and transition_ids are required.}

\item{adaptiveRTs}{(environment) For each descendant-pair, it contains the window around the aligned
retention time, within which features with m-score below aligned FDR are considered for quantification.}

\item{refRuns}{(environment) For each descendant-pair, the reference run is indicated by 1 or 2 for all the peptides.}

\item{params}{(list) parameters are entered as list. Output of the \code{\link{paramsDIAlignR}} function.}

\item{applyFun}{(function) value must be either lapply or BiocParallel::bplapply.}

\item{analytes}{(integer) this vector contains transition_group_id from precursors. It must be of
the same length as of multipeptide.}
}
\value{
(None)
}
\description{
Features of the root node are propagated to all leaves node. Aligned features are set/added in the
multipeptide environment.
}
\examples{
dataPath <- system.file("extdata", package = "DIAlignR")
params <- paramsDIAlignR()
fileInfo <- getRunNames(dataPath = dataPath)
mzPntrs <- list2env(getMZMLpointers(fileInfo))
features <- list2env(getFeatures(fileInfo, maxFdrQuery = params[["maxFdrQuery"]], runType = params[["runType"]]))
precursors <- getPrecursors(fileInfo, oswMerged = TRUE, runType = params[["runType"]],
 context = "experiment-wide", maxPeptideFdr = params[["maxPeptideFdr"]])
precursors <- dplyr::arrange(precursors, .data$peptide_id, .data$transition_group_id)
peptideIDs <- unique(precursors$peptide_id)
peptideScores <- getPeptideScores(fileInfo, peptideIDs, oswMerged = TRUE, params[["runType"]], params[["context"]])
peptideScores <- lapply(peptideIDs, function(pep) dplyr::filter(peptideScores, .data$peptide_id == pep))
names(peptideScores) <- as.character(peptideIDs)
prec2chromIndex <- list2env(getChromatogramIndices(fileInfo, precursors, mzPntrs))
multipeptide <- getMultipeptide(precursors, features)
adaptiveRTs <- new.env()
refRuns <- new.env()
tree <- ape::read.tree(text = "(run1:9,(run2:7,run0:2)master2:5)master1;")
tree <- ape::reorder.phylo(tree, "postorder")
\dontrun{
ropenms <- get_ropenms(condaEnv = "envName", useConda=TRUE)
multipeptide <- traverseUp(tree, dataPath, fileInfo, features, mzPntrs, prec2chromIndex, precursors, params,
 adaptiveRTs, refRuns, multipeptide, peptideScores, ropenms)
multipeptide <- getMultipeptide(precursors, features)
multipeptide <- traverseDown(tree, dataPath, fileInfo, multipeptide, prec2chromIndex, mzPntrs, precursors,
 adaptiveRTs, refRuns, params)
# Cleanup
rm(mzPntrs)
file.remove(list.files(dataPath, pattern = "*_av.rds", full.names = TRUE))
file.remove(list.files(file.path(dataPath, "xics"), pattern = "^master[0-9]+\\\\.chrom\\\\.mzML$", full.names = TRUE))
}
}
\seealso{
\code{\link{traverseUp}, \link{alignToMaster}}
}
\author{
Shubham Gupta, \email{shubh.gupta@mail.utoronto.ca}

ORCID: 0000-0003-3500-8152

License: (c) Author (2020) + GPL-3
Date: 2020-07-01
}
\keyword{internal}
