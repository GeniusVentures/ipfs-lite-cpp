
#include "data_transfer/message_receiver.hpp"

namespace sgns::data_transfer {

  IPFS::outcome::result<void> MessageReceiver::registerVoucherType(
      const std::string &type, std::shared_ptr<RequestValidator> validator) {
    auto res = voucher_validators_.try_emplace(type, validator);
    if (!res.second)
      return MessageReceiverError::VOUCHER_VALIDATOR_ALREADY_REGISTERED;

    return sgns::IPFS::outcome::success();
  }

  IPFS::outcome::result<void> MessageReceiver::validateVoucher(
      const PeerId &sender, const DataTransferRequest &request) const {
    auto validator = voucher_validators_.find(request.voucher_type);
    if (validator == voucher_validators_.end()) {
      return MessageReceiverError::VOUCHER_VALIDATOR_NOT_FOUND;
    }
    OUTCOME_TRY((auto &&, base_cid), CID::fromString(request.base_cid));
    OUTCOME_TRY((auto &&, selector), IPLDNodeImpl::createFromRawBytes(request.selector));
    if (request.is_pull) {
      BOOST_OUTCOME_TRYV2(auto &&, validator->second->validatePull(
          sender, request.voucher, base_cid, selector));
    } else {
      BOOST_OUTCOME_TRYV2(auto &&, validator->second->validatePush(
          sender, request.voucher, base_cid, selector));
    }

    return IPFS::outcome::success();
  }

}  // namespace sgns::data_transfer

OUTCOME_CPP_DEFINE_CATEGORY_3(sgns::data_transfer, MessageReceiverError, e) {
  using sgns::data_transfer::MessageReceiverError;

  switch (e) {
    case MessageReceiverError::VOUCHER_VALIDATOR_ALREADY_REGISTERED:
      return "MessageReceiverError: voucher validator is already "
             "registered";
    case MessageReceiverError::VOUCHER_VALIDATOR_NOT_FOUND:
      return "MessageReceiverError: voucher validator not found";
  }

  return "unknown error";
}
