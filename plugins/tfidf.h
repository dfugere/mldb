/** tfidf.h                                                          -*- C++ -*-
    Mathieu Marquis Bolduc, November 27th 2015
    Copyright (c) 2015 Datacratic Inc.  All rights reserved.

    This file is part of MLDB. Copyright 2015 Datacratic. All rights reserved.

    TF-IDF algorithm for a dataset.
*/

#pragma once

#include "mldb/core/dataset.h"
#include "mldb/core/procedure.h"
#include "mldb/core/function.h"
#include "matrix.h"
#include "mldb/ml/value_descriptions.h"
#include "mldb/types/optional.h"
#include "metric_space.h"

namespace Datacratic {
namespace MLDB {

enum TFType {
    TF_raw,
    TF_log,
    TF_augmented
};

enum IDFType {
    IDF_unary,
    IDF_inverse,
    IDF_inverseSmooth,
    IDF_inverseMax,
    IDF_probabilistic_inverse
};

DECLARE_ENUM_DESCRIPTION(TFType);
DECLARE_ENUM_DESCRIPTION(IDFType);

struct TfidfConfig : public ProcedureConfig {
    TfidfConfig()
        : select("*"),
          when(WhenExpression::TRUE),
          where(SqlExpression::TRUE),
          orderBy(ORDER_BY_NOTHING),
          offset(0),
          limit(-1)         
    {

    }

    std::shared_ptr<TableExpression> dataset;
    PolyConfigT<Dataset> output;
    static constexpr char const * defaultOutputDatasetType = "sparse.mutable";

    SelectExpression select;
    WhenExpression when;
    std::shared_ptr<SqlExpression> where;
    OrderByExpression orderBy;
    ssize_t offset;
    ssize_t limit; 

    Utf8String functionName;
};

DECLARE_STRUCTURE_DESCRIPTION(TfidfConfig);



/*****************************************************************************/
/* TFIDF PROCEDURE                                                           */
/*****************************************************************************/

struct TfidfProcedure: public Procedure {
    
    TfidfProcedure(MldbServer * owner,
                   PolyConfig config,
                   const std::function<bool (const Json::Value &)> & onProgress);

    virtual RunOutput run(const ProcedureRunConfig & run,
                          const std::function<bool (const Json::Value &)> & onProgress) const;

    virtual Any getStatus() const;

    TfidfConfig tfidfconfig;
};


/*****************************************************************************/
/* TFIDF FUNCTION                                                             */
/*****************************************************************************/

struct TfidfFunctionConfig {
    TfidfFunctionConfig(PolyConfigT<Dataset> dataset, int sizeOfCorpus = 1)
        : dataset(dataset),         
          N(sizeOfCorpus),
          tf_type(TF_log),
          idf_type(IDF_inverse)

    {
    }

    TfidfFunctionConfig()
        : N(1),
          tf_type(TF_log),
          idf_type(IDF_inverse)
    {
    }
    
    PolyConfigT<Dataset> dataset;       
    int N;
    TFType tf_type;
    IDFType idf_type;
};

DECLARE_STRUCTURE_DESCRIPTION(TfidfFunctionConfig);

struct TfidfFunction: public Function {
    TfidfFunction(MldbServer * owner,
                PolyConfig config,
                const std::function<bool (const Json::Value &)> & onProgress);
    
    virtual Any getStatus() const;
    
    virtual FunctionOutput apply(const FunctionApplier & applier,
                              const FunctionContext & context) const;
    
    /** Describe what the input and output is for this function. */
    virtual FunctionInfo getFunctionInfo() const;
    
    std::shared_ptr<Dataset> dataset;
    TfidfFunctionConfig functionConfig;
};

} // namespace MLDB
} // namespace Datacratic
