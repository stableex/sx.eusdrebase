#pragma once

#include <eosio/asset.hpp>

namespace eusd {

    using eosio::asset;
    using eosio::symbol;
    using eosio::extended_symbol;
    using eosio::name;
    using eosio::multi_index;

    // reference
    const name id = "eusd"_n;
    const name code = "eosrebaseusd"_n;
    const std::string description = "EUSDC Rebase";

    const extended_symbol EUSDC{ symbol{"EUSDC", 6}, "eoseusdtoken"_n };
    const extended_symbol EUSDB{ symbol{"EUSDB", 6}, "eoseusdtoken"_n };


    struct [[eosio::table]] global_row {
        double_t price_cumulative_last;
        uint64_t last_update;
        asset debt;
        uint64_t pair_id;
        uint64_t epoch_time;

        uint64_t primary_key() const { return pair_id; }
    };
    typedef eosio::multi_index< "global"_n, global_row > global;


    struct [[eosio::table]] oracles_row {
        double_t price_cumulative_last;
        uint64_t last_update;
        uint64_t pair_id;
        uint64_t update_min;
        uint64_t seg;

        uint64_t primary_key() const { return seg; }
    };
    typedef eosio::multi_index< "oracles"_n, oracles_row > oracles;

    /**
     * ## STATIC `get_amount_out`
     *
     * Return conversion amount
     *
     * ### params
     *
     * - `{asset} in` - EUSDC asset
     *
     * ### returns
     *
     * - `{asset}` - EUSDB asset
     *
     * ### example
     *
     * ```c++
     * const asset in = {10000000, symbol{"EUSDC",6}};
     *
     * const auto out = eusd::get_amount_out( in );
     * // out => "10.300000 EUSDB"
     * ```
     */
    static asset get_amount_out(const asset in )
    {
        if(in.symbol != EUSDC.get_symbol()) return {0, EUSDB.get_symbol() };

        //global _glob( code, code.value );
        oracles _oracles( code, code.value );

        const auto seg = 1;
        const auto ora = _oracles.get(seg, "eusdc: can't get oracle price");
        //auto epoch_time = _glob.begin()->epoch_time;
        const auto price = ora.price_cumulative_last / 120;
        const auto amount_out = price < 1 ? in.amount / price : 0;

        return asset{ static_cast<int64_t>(amount_out), EUSDB.get_symbol() };
    }


}