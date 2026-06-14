# DNS records at Spaceship for Firebase Hosting.
#
# These are static values — Firebase Hosting IPs are well-known and stable.
# The TXT records handle domain ownership verification and SSL provisioning.
# If Firebase ever changes its IPs (extremely unlikely), update here.
#
# NOTE: The `_acme-challenge` TXT value below is a placeholder. After the
# first `terraform apply`, copy the actual ACME challenge value from the
# Firebase Hosting console (or from `google_firebase_hosting_custom_domain`
# state under `dns_updates.discovered.records`) and re-apply.

resource "spaceship_dns_records" "site" {
  domain = local.domain

  records = [
    {
      type    = "A"
      name    = "@"
      address = "199.36.158.100"
      ttl     = 60
    },
    {
      type  = "TXT"
      name  = "@"
      value = "hosting-site=${local.project_id}"
      ttl   = 60
    },
    {
      type  = "TXT"
      name  = "_acme-challenge"
      value = "r3Fd6HdzTaWZl46yzHmQb49n7H9kfgjlVVF0D_mslXk"
      ttl   = 60
    },
  ]
}
