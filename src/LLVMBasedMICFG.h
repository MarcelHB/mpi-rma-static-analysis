/**
 * Modified version of Phasar file `LLVMBasedICFG.h`, initially written by:
 */

/******************************************************************************
 * Copyright (c) 2017 Philipp Schubert.
 * All rights reserved. This program and the accompanying materials are made
 * available under the terms of PHASAR_LICENSE.txt.
 *
 * Contributors:
 *     Philipp Schubert and others
 *****************************************************************************/

/**
 * Reasons to have such a modified `LLVMBasedICFG` implementation:
 *
 * The current class has limited extensibility, and we need a special resolver
 * class: `MOTFResolver`.
 *
 */

#ifndef H_LLVM_BASED_MICFG
#define H_LLVM_BASED_MICFG

#include <phasar/PhasarLLVM/ControlFlow/LLVMBasedICFG.h>

#include "MOTFResolver.h"

class LLVMBasedMICFG : public psr::LLVMBasedICFG {
  public:
    using Resolver_t = psr::Resolver;

  private:
    psr::LLVMTypeHierarchy &CH;
    psr::ProjectIRDB &IRDB;
    psr::PointsToGraph WholeModulePTG;
    std::unordered_set<const llvm::Function *> VisitedFunctions;

    // The VertexProperties for our call-graph.
    struct VertexProperties {
      const llvm::Function *function = nullptr;
      std::string functionName;
      bool isDeclaration;

      VertexProperties () = default;
      VertexProperties (const llvm::Function *f, bool isDecl = false);
    };

    // The EdgeProperties for our call-graph.
    struct EdgeProperties {
      const llvm::Instruction *callsite = nullptr;
      std::string ir_code;
      uint64_t id = 0;

      EdgeProperties () = default;
      EdgeProperties (const llvm::Instruction *i);
    };

    /// Specify the type of graph to be used.
    typedef boost::adjacency_list<
        boost::multisetS, boost::vecS
      , boost::bidirectionalS, VertexProperties
      , EdgeProperties
    > bidigraph_t;

    // Let us have some handy typedefs.
    typedef boost::graph_traits<bidigraph_t>::vertex_descriptor vertex_t;
    typedef boost::graph_traits<bidigraph_t>::vertex_iterator vertex_iterator;
    typedef boost::graph_traits<bidigraph_t>::edge_descriptor edge_t;
    typedef boost::graph_traits<bidigraph_t>::out_edge_iterator out_edge_iterator;
    typedef boost::graph_traits<bidigraph_t>::in_edge_iterator in_edge_iterator;

    /// The call graph.
    bidigraph_t cg;

    /// Maps function names to the corresponding vertex id.
    std::unordered_map<std::string, vertex_t> function_vertex_map;

    void constructionWalker (const llvm::Function *F, MOTFResolver *resolver);

    struct dependency_visitor;

  public:
    LLVMBasedMICFG (
        psr::LLVMTypeHierarchy &STH
      , psr::ProjectIRDB &IRDB
      , const std::vector<std::string> &EntryPoints
    );

    virtual ~LLVMBasedMICFG () = default;

    bool isVirtualFunctionCall (llvm::ImmutableCallSite CS);

    const llvm::Function *getMethodOf (const llvm::Instruction *stmt) override;

    std::vector<const llvm::Instruction *>
    getPredsOf (const llvm::Instruction *I) override;

    std::vector<const llvm::Instruction *>
    getSuccsOf (const llvm::Instruction *I) override;

    std::vector<std::pair<const llvm::Instruction *, const llvm::Instruction *>>
    getAllControlFlowEdges (const llvm::Function *fun) override;

    std::set<const llvm::Function *> getAllMethods ();

    std::vector<const llvm::Instruction *>
    getAllInstructionsOf (const llvm::Function *fun) override;

    bool isExitStmt (const llvm::Instruction *stmt) override;

    bool isStartPoint (const llvm::Instruction *stmt) override;

    bool isFallThroughSuccessor (
        const llvm::Instruction *stmt
      , const llvm::Instruction *succ
    ) override;

    bool isBranchTarget(
        const llvm::Instruction *stmt
      , const llvm::Instruction *succ
    ) override;

    std::string getMethodName (const llvm::Function *fun) override;

    std::string getStatementId (const llvm::Instruction *stmt) override;

    const llvm::Function *getMethod (const std::string &fun) override;

    std::set<const llvm::Function *>
    getCalleesOfCallAt (const llvm::Instruction *n) override;

    std::set<const llvm::Instruction *>
    getCallersOf (const llvm::Function *m) override;

    std::set<const llvm::Instruction *>
    getCallsFromWithin (const llvm::Function *m) override;

    std::set<const llvm::Instruction *>
    getStartPointsOf (const llvm::Function *m) override;

    std::set<const llvm::Instruction *>
    getExitPointsOf (const llvm::Function *fun) override;

    std::set<const llvm::Instruction *>
    getReturnSitesOfCallAt (const llvm::Instruction *n) override;

    bool isCallStmt (const llvm::Instruction *stmt) override;

    std::set<const llvm::Instruction *> allNonCallStartNodes () override;

    const llvm::Instruction *getLastInstructionOf (const std::string &name);

    std::vector<const llvm::Instruction *>
    getAllInstructionsOfFunction (const std::string &name);

    void mergeWith (const LLVMBasedMICFG &other);

    bool isPrimitiveFunction (const std::string &name);

    void print ();

    void printAsDot (const std::string &filename);

    // This does not work, as only the parent is `friend`
    // enough to access the internal graph:
    // void printInternalPTGAsDot (const std::string &filename);

    unsigned getNumOfVertices ();

    unsigned getNumOfEdges ();

    void exportPATBCJSON ();

    psr::PointsToGraph &getWholeModulePTG ();

    std::vector<std::string> getDependencyOrderedFunctions ();
};

#endif
